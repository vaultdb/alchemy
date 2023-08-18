#ifndef _PLAN_ENUMERATOR_
#define _PLAN_ENUMERATOR_
#include <operators/operator.h>

namespace vaultdb {
    class PlanEnumerator {
    public:
        // traverse a left-deep tree and ID a minimum-cost plan
        // does not change order of operators, only varies their implementation and the placement of needed sorts
        PlanEnumerator(SecureOperator *root, const map<int, vector<SortDefinition >> & sort_orders);
        SecureOperator *optimizeTree();



    private:
        map<int, vector<SortDefinition >> interesting_sort_orders_;
        size_t min_cost_ = std::numeric_limits<size_t>::max();
        SecureOperator *min_cost_plan_ = nullptr;
        map<int, SecureOperator *> tree_nodes_;
        SecureOperator *root_;

        void setup(SecureOperator *op);
        void optimizeTreeHelper(SecureOperator *op);
        void recurseJoin(SecureOperator *join);
        void recurseAgg(SecureOperator *agg);
        void recurseNode(SecureOperator *op);
        inline SecureOperator *fetchLeaf(SecureOperator *op) {
            while(!op->isLeaf()) {
                op = op->getChild();
            }
            return op;
        }

        inline vector<SortDefinition> getCollations(SecureOperator *op) {
            map<SortDefinition, int> collations; // making a map to eliminate duplicate collations
            collations[SortDefinition()] = 0; // empty set
            collations[op->getSortOrder()] = 0; // default sort
            for (auto collation: interesting_sort_orders_[op->getOperatorId()]) {
                collations[collation] = 0;
            }

            vector<SortDefinition> sorts;
            for (auto collation: collations) {
                sorts.push_back(collation.first);
            }
            return sorts;
        }





    };
}

#endif
