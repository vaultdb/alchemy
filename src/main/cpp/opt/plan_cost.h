#ifndef _PLAN_COST_
#define _PLAN_COST_
#include "operators/operator.h"
#include "operator_cost_model.h"

namespace vaultdb {
    class PlanCost {
        // takes in the root operator for a tree
        // produces a cost estimate for the plan in gates
        // this is a very simple model that just counts the number of gates
        static size_t planCost(const Operator<Bit> *root) {
            size_t gate_cost = 0;
            if(!root->isLeaf()) {
                gate_cost += planCost(root->getChild(0));
                auto rhs = root->getChild(1);
                if(rhs != nullptr) {
                    gate_cost += planCost(rhs);
                }
            }
            gate_cost += OperatorCostModel::operatorCost(root);
            return gate_cost;
        }


    };
}
#endif
