#ifndef _EXPRESSION_PARSER_H
#define _EXPRESSION_PARSER_H

#include <expression/expression.h>
#include <expression/generic_expression.h>
#include <expression/expression_node.h>
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include <string>
#include <vector>

using boost::property_tree::ptree;
// converts from Apache Calcite's JSON representation to a GenericExpression or BinaryExpression

namespace vaultdb {
template<typename B>
class ExpressionParser {
public:
    static Expression <B> * parseJSONExpression(const std::string &json, const QuerySchema & input_schema);
    static Expression <B> * parseJSONExpression(const std::string &json, const QuerySchema & lhs, const QuerySchema & rhs);
    static Expression <B> * parseExpression(const ptree & tree, const QuerySchema & lhs, const QuerySchema & rhs = QuerySchema());

private:
    static ExpressionNode <B> * parseHelper(const ptree & tree, const QuerySchema & lhs, const QuerySchema & rhs = QuerySchema());
    static ExpressionNode <B> * parseSubExpression(const ptree & tree, const QuerySchema & lhs, const QuerySchema & rhs = QuerySchema());
    static ExpressionNode <B> * parseInput(const ptree & tree, const QuerySchema & lhs, const QuerySchema & rhs = QuerySchema());

};
} // namespace vaultdb

#endif //_EXPRESSION_PARSER_H
