#ifndef _COMMON_TABLE_EXPRESSION_H
#define _COMMON_TABLE_EXPRESSION_H


#include "operator.h"

namespace vaultdb {
    // used for adding arbitrary tables as inputs, as in common table expressions (aka WITH clauses)
    class CommonTableExpression : public Operator {

    public:
        CommonTableExpression(const std::shared_ptr<QueryTable> & inputTable) { output = std::move(inputTable); }
        std::shared_ptr<QueryTable> runSelf() override;

    };
}


#endif //VAULTDB_EMP_COMMON_TABLE_EXPRESSION_H
