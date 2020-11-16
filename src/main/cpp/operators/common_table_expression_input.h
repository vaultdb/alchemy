#ifndef _COMMON_TABLE_EXPRESSION_INPUT_H
#define _COMMON_TABLE_EXPRESSION_INPUT_H


#include "operator.h"

namespace vaultdb {
    // used for adding arbitrary tables as inputs, as in common table expressions (aka WITH clauses)
    class CommonTableExpressionInput : public Operator {

    public:
        CommonTableExpressionInput(const std::shared_ptr<QueryTable> & inputTable) { output = std::move(inputTable); }
        std::shared_ptr<QueryTable> runSelf() override;

    };
}


#endif //VAULTDB_EMP_COMMON_TABLE_EXPRESSION_INPUT_H
