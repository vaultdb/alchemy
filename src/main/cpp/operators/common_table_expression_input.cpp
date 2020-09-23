//
// Created by Jennie Rogers on 9/14/20.
//

#include "common_table_expression_input.h"

std::shared_ptr<QueryTable> CommonTableExpressionInput::runSelf() {
    return output;
}
