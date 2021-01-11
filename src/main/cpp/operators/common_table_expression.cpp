#include "common_table_expression.h"

using namespace vaultdb;

std::shared_ptr<QueryTable> CommonTableExpression::runSelf() {
    return output;
}
