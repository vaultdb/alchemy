#include "expression_factory.h"
#include "function_expression.h"

using namespace vaultdb;

template<typename B>
ExpressionNode<B> *ExpressionFactory<B>::getExpressionNode(const string &expression_kind, ExpressionNode<B> *lhs,
ExpressionNode<B> *rhs) {
    ExpressionKind kind = getKind(expression_kind);
    return getExpressionNode(kind, lhs, rhs);
}

template<typename B>
ExpressionNode<B> *ExpressionFactory<B>::getExpressionNode(const string &expression_kind, vector<ExpressionNode<B> * >  & operands) {
    ExpressionKind kind = getKind(expression_kind);

    if(kind == ExpressionKind::CASE) {
        assert(operands.size() == 3);
        BoolExpression<B> conditional(operands[0]);
        return new CaseNode<B>(conditional, operands[1], operands[2]);
    }
    else
        return getExpressionNode(kind, operands[0], operands[1]);
}


template<typename B>
ExpressionNode<B> *ExpressionFactory<B>::getExpressionNode(const ExpressionKind &kind, ExpressionNode<B> *lhs,
ExpressionNode<B> *rhs) {
    switch(kind) {
        case ExpressionKind::PLUS:
            return new PlusNode<B>(lhs, rhs);
        case ExpressionKind::MINUS:
            return new MinusNode<B>(lhs, rhs);
        case ExpressionKind::TIMES:
            return new TimesNode<B>(lhs, rhs);
        case ExpressionKind::DIVIDE:
            return new DivideNode<B>(lhs, rhs);
        case ExpressionKind::MOD:
            return new ModulusNode<B>(lhs, rhs);

        case ExpressionKind::AND:
            return new AndNode<B>(lhs, rhs);
        case ExpressionKind::OR:
            return new OrNode<B>(lhs, rhs);
        case ExpressionKind::NOT:
            return new NotNode<B>(lhs);

        case ExpressionKind::EQ:
            return new EqualNode<B>(lhs, rhs);
        case ExpressionKind::NEQ:
            return new NotEqualNode<B>(lhs, rhs);
        case ExpressionKind::LT:
            return new LessThanNode<B>(lhs, rhs);
        case ExpressionKind::LEQ:
            return new LessThanEqNode<B>(lhs, rhs);
        case ExpressionKind::GT:
            return new GreaterThanNode<B>(lhs, rhs);
        case ExpressionKind::GEQ:
            return new GreaterThanEqNode<B>(lhs, rhs);
        default:
            throw new std::invalid_argument("Can't create ExpressionNode for kind " + std::to_string((int) kind));
    }
}

template<typename B>
ExpressionKind ExpressionFactory<B>::getKind(const string &expression_kind) {
    if(expression_kind == "LITERAL") return ExpressionKind::LITERAL;
    if(expression_kind == "INPUT") return ExpressionKind::INPUT_REF;


    if(expression_kind == "PLUS") return ExpressionKind::PLUS;
    if(expression_kind == "MINUS") return ExpressionKind::MINUS;
    if(expression_kind == "TIMES") return ExpressionKind::TIMES;
    if(expression_kind == "DIVIDE") return ExpressionKind::DIVIDE;
    if(expression_kind == "MODULUS") return ExpressionKind::MOD;

    if(expression_kind == "AND") return ExpressionKind::AND;
    if(expression_kind == "OR") return ExpressionKind::OR;
    if(expression_kind == "NOT") return ExpressionKind::NOT;

    if(expression_kind == "EQUALS") return ExpressionKind::EQ;
    if(expression_kind == "NOT_EQUALS") return ExpressionKind::NEQ;
    if(expression_kind == "GREATER_THAN") return ExpressionKind::GT;
    if(expression_kind == "GREATER_THAN_OR_EQUAL") return ExpressionKind::GEQ;
    if(expression_kind == "LESS_THAN") return ExpressionKind::LT;
    if(expression_kind == "LESS_THAN_OR_EQUAL") return ExpressionKind::LEQ;
    if(expression_kind == "CASE") return ExpressionKind::CASE;

    throw new std::invalid_argument("Can't create ExpressionKind for  " + expression_kind);


}




template class vaultdb::ExpressionFactory<bool>;
template class vaultdb::ExpressionFactory<emp::Bit>;
