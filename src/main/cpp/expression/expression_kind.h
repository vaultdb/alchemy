#ifndef _EXPRESSION_KIND_H
#define _EXPRESSION_KIND_H

enum class ExpressionKind {
    INPUT_REF,
    PACKED_INPUT_REF,
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
    FUNCTION, // for FunctionExpression
    CAST,
    CASE,
    NO_OP,
    UNDEFINED

};

enum class ExpressionClass {
    GENERIC,
    BOOL,
    FUNC
};

#endif //_EXPRESSION_KIND_H
