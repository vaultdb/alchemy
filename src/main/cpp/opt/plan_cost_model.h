#ifndef __PLAN_COST_MODEL_
#define __PLAN_COST_MODEL_
#include "operators/operator.h"

namespace vaultdb {
    class PlanCostModel {
        // takes in the root operator for a tree
        // produces a cost estimate for the plan in gates
        // this is a very simple model that just counts the number of gates
        PlanCostModel(const SecureOperator *root) {

        }
    };
}
#endif
