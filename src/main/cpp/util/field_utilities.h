#ifndef _FIELD_UTILITIES_H
#define _FIELD_UTILITIES_H


#include<type_traits>
#include <map>
#include <query_table/field/field.h>
#include <query_table/query_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/field/field_type.h"
#include "common/defs.h"
#include "expression/expression_node.h"
#include "expression/comparator_expression_nodes.h"
#include "expression/generic_expression.h"
#include <expression/visitor/to_packed_expression_visitor.h>


namespace vaultdb {

    template <typename B> class Operator;

    class FieldUtilities {
    public:
        // size is in bytes
        static size_t getPhysicalSize(const FieldType &id, const size_t & str_length = 0);
        static emp::Float toFloat(const emp::Integer &input);

        template<typename B>
        static inline bool validateTypeAlignment(const Field<B> &field) {

            switch(field.getType()) {
                case FieldType::BOOL:
                    return field.payload_.which() == 0;
                case FieldType::INT:
                    return field.payload_.which() == 1;
                case FieldType::LONG:
                    return field.payload_.which() == 2;
                case FieldType::FLOAT:
                    return field.payload_.which() == 3;
                case FieldType::STRING:
                    return field.payload_.which() == 4;
                case FieldType::SECURE_BOOL:
                    return field.payload_.which() == 5;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING:
                    return field.payload_.which() == 6;
                case FieldType::SECURE_FLOAT:
                    return field.payload_.which() == 7;
                default:
                    return true; // uninitialized for FieldType::INVALID

            }

        }

        static void secret_share_send(const PlainTable *src, const int &src_idx, SecureTable *dst, const int &dst_idx,
                                      const int &party);

//        static void secret_share_send(const QueryTuple<bool> &src_tuple, QueryTuple<Bit> &dst_tuple, const int &party);
//        static void secret_share_recv(SecureTuple &dst_tuple, const int &party);

        static void secret_share_recv(SecureTable *dst, const int & idx, const int &party);


        // for template<typename T> case
        static bool extract_bool(const emp::Bit & b) { return b.reveal();  }
        static bool extract_bool(const bool & b) { return b; }

//        static PlainTuple revealTuple(const PlainTuple & p) {return p; }
//        static PlainTuple revealTuple(const SecureTuple  & s);

        // expected size in bits
        inline static int getExpectedSize(const FieldType & t, int str_length) {
            return TypeUtilities::getTypeSize(t) * ((t == FieldType::SECURE_STRING) ? str_length : 1);
        }

        static inline Field<bool> getBoolField(const bool & input) { return Field<bool>(FieldType::BOOL, input); }
        static inline Field<emp::Bit> getBoolField(const emp::Bit & input) { return Field<emp::Bit>(FieldType::SECURE_BOOL, input); }

        static std::string printTupleBits(const PlainTuple & p);
        static std::string printTupleBits(const SecureTuple & s);

        // compatible with OMPC
        static string printTupleBits(const QueryTable<Bit> *table, const int & idx) {
            stringstream out;
            out << "(";
            if(table->getSchema().getFieldCount() > 0) {
                SecureField f = table->getField(idx, 0);
                out << FieldUtilities::printField(f);
                for (int j = 1; j < table->getSchema().getFieldCount(); ++j) {
                    f = table->getField(idx, j);
                    out << ", " << FieldUtilities::printField(f);
                }
            }
            out << ")";
            // dummy tag
            out << " (dummy=" << table->getDummyTag(idx).reveal() << ")";
           return out.str();
        }

        static std::string printTupleBits(const QueryTable<bool> *table, const int & idx) {
            return printTupleBits(table->getPlainTuple(idx));
        }

        static bool getBoolPrimitive(const Field<bool> & input) { return input.getValue<bool>(); }
        static emp::Bit getBoolPrimitive(const Field<emp::Bit> & input) { return input.getValue<emp::Bit>(); }

        static bool select(const bool & choice, const bool & lhs, const bool & rhs) {
            return choice ? lhs : rhs;
        }
        static emp::Bit select(const emp::Bit & choice, const emp::Bit & lhs, const emp::Bit & rhs) {
            return emp::If(choice, lhs, rhs);
        }

        static BitPackingMetadata getBitPackingMetadata(const std::string & db_name);





        template<typename B>
        static  Expression<B> *
        getEqualityPredicate(const uint32_t &lhs_idx, const QuerySchema &lhs, const uint32_t &rhs_idx,
                             const QuerySchema & rhs) {

            auto lhs_input = new InputReference<B>(lhs_idx, lhs,  rhs);
            auto rhs_input = new InputReference<B>(rhs_idx, lhs,  rhs);
            auto equality_node = new EqualNode<B>(lhs_input, rhs_input);

            ToPackedExpressionVisitor pack_it(equality_node);
            ExpressionNode<B> *packed_predicate = pack_it.getRoot();

            GenericExpression<B> *g =  new GenericExpression<B>(packed_predicate, "predicate",
                                                                std::is_same_v<B, bool> ? FieldType::BOOL : FieldType::SECURE_BOOL);
            delete equality_node;
            delete lhs_input;
            delete rhs_input;
            return g;

        }

        // for use in joins
        // indexes are based on the concatenated tuple, not addressing each input to the join comparison individually
        template<typename B>
        static Expression<B> *
        getEqualityPredicate(const Operator<B> *lhs, const uint32_t &lhs_idx, const Operator<B> *rhs,
                             const uint32_t &rhs_idx) {

            return FieldUtilities::getEqualityPredicate<B>(lhs_idx, lhs->getOutputSchema(), rhs_idx, rhs->getOutputSchema());

        }

        template<typename B>
        static ExpressionNode<B> *getEqualityPredicateNode(const Operator<B> *lhs, const uint32_t &lhs_idx, const Operator<B> *rhs,
                             const uint32_t &rhs_idx) {


            auto lhs_input = new InputReference<B>(lhs_idx, lhs->getOutputSchema(),  rhs->getOutputSchema());
            auto rhs_input = new InputReference<B>(rhs_idx, lhs->getOutputSchema(),  rhs->getOutputSchema());
            auto equality_node = new EqualNode<B>(lhs_input, rhs_input);

            ToPackedExpressionVisitor pack_it(equality_node);
            return pack_it.getRoot();

        }


        template<typename B>
        static string revealAndPrintTuple(QueryTable<B> *table, const int & idx) {
            stringstream ss;
            ss << "(";
            for (int i = 0; i < table->getSchema().getFieldCount(); ++i) {
                PlainField f = table->getField(idx, i).reveal(table->getSchema().getField(i));
                ss << f.toString();
                if (i < table->getSchema().getFieldCount() - 1) ss << ", ";
            }
            ss << ") (dummy=" << FieldUtilities::extract_bool(table->getDummyTag(idx)) << ")";
            return ss.str();
        }

        static string printFloat(const float_t & f);
        static string printFloat(const Float & f);
        static string printInt(const int32_t & i);
        static string printInt(const Integer & i);
        static string printString(const string & s);

        static string printField(const SecureField & f) {
            if(f.getType() == FieldType::SECURE_FLOAT) {
                return printFloat(f.getFloat());
            }
            if(f.getType() == FieldType::SECURE_BOOL) {
                stringstream ss;
                bool b = f.getValue<emp::Bit>().reveal();
                ss << b << ": " << (b ? "true" : "false");
                return ss.str();
            }
            return printInt(f.getInt());

        }

        static string printField(const PlainField & f) {
            if(f.getType() == FieldType::FLOAT) {
                return printFloat(f.getValue<float_t>());
            }
            if(f.getType() == FieldType::BOOL) {
                stringstream ss;
                bool b = f.getValue<bool>();
                ss << b << ": " << (b ? "true" : "false");
                return ss.str();
            }
            if(f.getType() == FieldType::STRING) {
                string s = f.getString();
                return printString(s);
            }

            if(f.getType() == FieldType::INT)
                return printInt(f.getValue<int32_t>());

            if(f.getType() == FieldType::LONG)
                return printInt(f.getValue<int64_t>());

            throw; // other types N/A

        }


    };


}
#endif //_FIELD_UTILITIES_H
