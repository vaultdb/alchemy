#include "parser/expression_parser.h"
#include <boost/property_tree/json_parser.hpp>
#include <util/data_utilities.h>
#include <expression/expression_factory.h>
#include <expression/visitor/type_validation_visitor.h>
#include <expression/visitor/to_packed_expression_visitor.h>

using namespace vaultdb;
using namespace std;

template<typename B>
Expression<B> * ExpressionParser<B>::parseJSONExpression(const std::string &json, const QuerySchema & input_schema) {
    stringstream ss;
    ss << json << endl;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    return parseExpression(pt, input_schema);
}

template<typename B>
Expression<B> * ExpressionParser<B>::parseJSONExpression(const std::string &json, const QuerySchema & lhs, const QuerySchema & rhs) {
    stringstream ss;
    ss << json << endl;
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    return parseExpression(pt, lhs, rhs);
}

template<typename B>
Expression<B> * ExpressionParser<B>::parseExpression(const ptree &tree, const QuerySchema & lhs, const QuerySchema & rhs) {

    ExpressionNode<B> *expression_root = parseHelper(tree, lhs, rhs);
    QuerySchema input_schema = QuerySchema::concatenate(lhs, rhs);
    TypeValidationVisitor<B> visitor(expression_root, input_schema);
    expression_root->accept(&visitor);

    GenericExpression<B> *g;
    if(expression_root->kind() == ExpressionKind::INPUT_REF && std::is_same_v<B, emp::Bit>) {
        InputReference<B> *input_ref = (InputReference<B> *) expression_root;
        auto packed = new PackedInputReference<B>(*input_ref);
        delete input_ref;
        expression_root = packed;
        g = new GenericExpression<B>(expression_root, input_schema);
    } else {
        ToPackedExpressionVisitor<B> pack_it(expression_root);
        expression_root->accept(&pack_it);
        g = new GenericExpression<B>(pack_it.getRoot(), input_schema);
    }

    delete expression_root;
    return g;

}

template<typename B>
ExpressionNode<B> * ExpressionParser<B>::parseHelper(const ptree &tree, const QuerySchema & lhs, const QuerySchema & rhs) {

    if((tree.count("op") > 0 && tree.count("operands") > 0))
       return parseSubExpression(tree, lhs, rhs);

    return parseInput(tree, lhs, rhs);

}

template<typename B>
ExpressionNode<B> * ExpressionParser<B>::parseSubExpression(const ptree &tree, const QuerySchema & lhs, const QuerySchema & rhs) {
    ptree op = tree.get_child("op");
    ptree operands = tree.get_child("operands");

    string op_name = op.get_child("kind").template get_value<string>();

    std::vector<ExpressionNode<B> *> children;

    // iterate over operands, invoke helper on each one
    for (ptree::const_iterator it = operands.begin(); it != operands.end(); ++it) {
        ExpressionNode<B> *t = parseHelper(it->second, lhs, rhs);
        children.push_back(t);
    }
    // op name is all upppercase
    std::transform(op_name.begin(), op_name.end(), op_name.begin(), ::toupper);
    // CAST has one additional arg in Calcite: type
    ExpressionNode<B> *res = nullptr;
    if(op_name == "CAST") {
        ptree type = tree.get_child("type");
        std::string type_str = type.get_child("type").template get_value<std::string>();
        FieldType dst_type = TypeUtilities::getTypeFromString(type_str);
        // Calcite only knows plaintext types, need to convert to secure
        if(std::is_same_v<B, emp::Bit>) dst_type = TypeUtilities::toSecure(dst_type);
        // sanity checking
        if(std::is_same_v<B, bool>) dst_type = TypeUtilities::toPlain(dst_type);
        res = new CastNode<B>(children[0], dst_type);
    }
    else res = ExpressionFactory<B>::getExpressionNode(op_name, children);

    for(ExpressionNode<B> *child : children) {
        delete child;
    }

    return res;

}

template<typename B>
ExpressionNode<B> * ExpressionParser<B>::parseInput(const ptree &tree, const QuerySchema & lhs, const QuerySchema & rhs) {

    if(tree.count("literal")  > 0) {
        ptree literal = tree.get_child("literal");
        std::string type_str = tree.get_child("type").get_child("type").template get_value<std::string>();

        if(type_str == "INTEGER" || type_str == "DATE") { // dates input as epochs
            int32_t literal_int = literal.template get_value<int32_t>();

            Field<B> input_field = (std::is_same_v<B, bool>) ?
                                   Field<B>(FieldType::INT, literal_int)
                                                             :  Field<B>(FieldType::SECURE_INT, emp::Integer(32, literal_int));
            return new LiteralNode<B>(input_field);
        } else if(type_str == "LONG") {
            int64_t literal_int = literal.template get_value<int64_t>();

            Field<B> input_field = (std::is_same_v<B, bool>) ?
                                   Field<B>(FieldType::LONG, literal_int)
                                                             :  Field<B>(FieldType::SECURE_LONG, emp::Integer(64, literal_int));
            return new LiteralNode<B>(input_field);
        } else if(type_str == "FLOAT" || type_str == "DECIMAL") {
            float_t literal_float = literal.template get_value<float_t>();
            Field<B> input_field = (std::is_same_v<B, bool>) ?
                                   Field<B>(FieldType::FLOAT, literal_float)
                                                             :  Field<B>(FieldType::SECURE_FLOAT, emp::Float(literal_float));

            return new LiteralNode<B>(input_field);


        } else if(type_str == "CHAR" || type_str == "VARCHAR") {
            boost::property_tree::ptree type_tree = tree.get_child("type");
            int length =    type_tree.get_child("precision").get_value<int>();
            std::string literal_string =   literal.template get_value<std::string>();
            while(literal_string.size() < length) literal_string += " ";
            Field<B> input_field =  Field<B>(FieldType::STRING, literal_string);

            if(std::is_same_v<B, emp::Bit>) {
                int sending_party = 1;
                Integer input_int;
                // sender
                if (SystemConfiguration::getInstance().party_ == sending_party) {
                     input_int = Field<B>::secretShareString(literal_string, true, sending_party, length);
                } else {
                    string placeholder(length, 0);
                    input_int = Field<B>::secretShareString(placeholder, false, sending_party, length);
                }

                return new LiteralNode<B>(Field<B>(FieldType::SECURE_STRING, input_int));
            }

            return new LiteralNode<B>(input_field);

        } else if(type_str == "BOOL" || type_str == "BOOLEAN") {
            bool literal_bool = literal.template get_value<bool>();
            Field<B> input_field = (std::is_same_v<B, bool>) ?  Field<B>(FieldType::BOOL, literal_bool) :
                                   Field<B>(FieldType::SECURE_BOOL, emp::Bit(literal_bool));
            return new LiteralNode<B>(input_field);
        }

        throw std::invalid_argument("Parsing input of type " + type_str + " not yet implemented!");

    }
    if(tree.count("no-op")  > 0) {
        return new NoOp<B>();
    }

    // else it is an input
    std::string expr = tree.get_child("input").get_value<std::string>();
    uint32_t src_ordinal;
    if(DataUtilities::isOrdinal(expr)) { // mapping a column from one position to another
        src_ordinal = std::atoi(expr.c_str());
    } else {
        throw std::invalid_argument("Expression " + expr + " is not a properly formed input");
    }

   return new InputReference<B>(src_ordinal, lhs, rhs);
}

template class vaultdb::ExpressionParser<bool>;
template class vaultdb::ExpressionParser<emp::Bit>;
