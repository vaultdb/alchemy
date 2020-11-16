#include "common_table_expression_input.h"

using namespace vaultdb;

std::shared_ptr<QueryTable> CommonTableExpressionInput::runSelf() {
    return output;
}
