#ifndef _EXPRESSION_KIND_H
#define _EXPRESSION_KIND_H
enum class ExpressionKind {
    UNDEFINED = 0,
    INPUT_REF,
    LITERAL,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MOD,
    EQ,
    NEQ,
    LT,
    LEQ,
    GT,
    GEQ,
    AND,
    OR,
    NOT,
    FUNCTION // for FunctionExpression
};

#endif //_EXPRESSION_KIND_H
