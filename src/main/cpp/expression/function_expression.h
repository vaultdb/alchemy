#ifndef _FUNCTION_EXPRESSION_H
#define _FUNCTION_EXPRESSION_H

#include "expression.h"

namespace vaultdb {

    template <typename B> class QueryTable;

    // TODO: make this inherit from ExpressionNode so we can include it in add'l expressions
    template<typename B>
    class FunctionExpression : public Expression<B> {

    public:
        FunctionExpression(Field<B> (*tuple_ptr)(const QueryTuple<B> &),
                           Field<B> (*table_ptr)(const QueryTable<B> *, const int &),
                           const std::string & anAlias, const FieldType & aType) : Expression<B>(anAlias, aType) {
            tuple_func_ = tuple_ptr;
            table_func_ = table_ptr;
        }


        Field<B> call(const QueryTuple<B> & aTuple) const override  {
            return tuple_func_(aTuple);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
              return   table_func_(src, row);
        }

        // binary prototype not yet implemented
        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            return  table_func_(lhs, lhs_row);
        }

        ExpressionKind kind() const override { return ExpressionKind::FUNCTION; }
        ExpressionClass exprClass() const override { return ExpressionClass::FUNC; }



        ~FunctionExpression() = default;

        string toString() const override {
            return "FunctionExpression(" + Expression<B>::alias_ + " " + TypeUtilities::getTypeString(Expression<B>::type_) + ")";

        }

    private:
        // function pointer to expression
        Field<B> (*tuple_func_)(const QueryTuple<B> &) = nullptr;
        Field<B> (*table_func_)(const QueryTable<B> *, const int &) = nullptr;


    };
}



#endif //VAULTDB_EMP_FUNCTION_EXPRESSION_H
