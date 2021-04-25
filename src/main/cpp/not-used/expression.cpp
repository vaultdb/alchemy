#include "operators/support/expression.h"



types::TypeId Expression::getType() const {
    return expressionType;
}

std::string Expression::getAlias() const {
    return alias;
}

bool Expression::isPrivate() const {
    return isPrivateField;
}

// TODO: override this in implementing classes
/*types::Value Expression::expressionCall(const QueryTuple &aTuple) const {
    throw; // we should never get here
}*/
