#ifndef _OPERATOR_COST_MODEL_
#define _OPERATOR_COST_MODEL_
#include <operators/operator.h>

#include "operators/basic_join.h"
#include "operators/filter.h"
#include "operators/sort_merge_aggregate.h"
#include "operators/keyed_join.h"
#include "operators/merge_input.h"
#include "operators/nested_loop_aggregate.h"
#include "operators/project.h"
#include "operators/secure_sql_input.h"
#include "operators/sort.h"
#include "operators/keyed_sort_merge_join.h"
#include "operators/sort_merge_join.h"
#include "operators/merge_join.h"
#include "operators/scalar_aggregate.h"
#include "operators/shrinkwrap.h"
#include "operators/union.h"
#include "operators/zk_sql_input.h"
#include <utility>
#include <vector>


namespace vaultdb {
class OperatorCostModel {
public:
    static size_t operatorCost(const SecureOperator *op);
    static size_t filterCost(const Filter<Bit>  *filter);
    static size_t secureSqlInputCost(const SecureSqlInput *input);
    static size_t projectCost(const Project<Bit> *project);
    static size_t basicJoinCost(const BasicJoin<Bit> *join);
    static size_t keyedJoinCost(const KeyedJoin<Bit> *join);
    static size_t keyedSortMergeJoinCost(KeyedSortMergeJoin<Bit> *join);
    static size_t mergeJoinCost(MergeJoin<Bit> *join);
    static size_t groupByAggregateCost(const SortMergeAggregate<Bit> *aggregate);
    static size_t nestedLoopAggregateCost(const NestedLoopAggregate<Bit> *aggregate);
    static size_t sortCost(const Sort<Bit> *sort);
    static size_t shrinkwrapCost(const Shrinkwrap<Bit> *shrinkwrap);
    static size_t scalarAggregateCost(const ScalarAggregate<Bit> *aggregate);
    static size_t mergeInputCost(const MergeInput *input);

    // these ones cost zero gates:
    //        static size_t unionCost(const Union<Bit> *union_op);
    //        static size_t zkSqlInputCost(const ZkSqlInput *input);

    // recursively calculate e2e plan cost
    static size_t planCost(const Operator<Bit> *root) {
        size_t cum_cost = 0L;
        planCostHelper(root, cum_cost);
        return cum_cost;
    }

    // estimate max memory usage of the plan
    static size_t operatorMemory(const SecureOperator *op) {
        if(op->isLeaf()) {
            return op->getOutputCardinality() * op->getOutputSchema().size();
        }

        // for project, if is does not have expression to execute, we only memcpy the input to output
        if (op->getType() == OperatorType::PROJECT) {
            Project<Bit> *project = (Project<Bit> *) op;
            if(project->getExpressionsToExec().size() == 0) {
                return 0;
            }
        }

        size_t lhs_child_bit_cnt = op->getChild(0)->getOutputCardinality() * op->getChild(0)->getOutputSchema().size();
        size_t rhs_child_bit_cnt = op->getChild(1) == NULL ? 0 : op->getChild(1)->getOutputCardinality() * op->getChild(1)->getOutputSchema().size();
        size_t children_memory = (lhs_child_bit_cnt + rhs_child_bit_cnt) * sizeof(emp::Bit);

        size_t output_bit_cnt = op->getOutputCardinality() * op->getOutputSchema().size();
        size_t output_memory = output_bit_cnt * sizeof(emp::Bit);

        return children_memory + output_memory;
    }

    static size_t planPeakMemory(const Operator<Bit> *root) {
        size_t peak_memory = 0L;
        planPeakMemoryHelper(root, peak_memory);
        return peak_memory;
    }


private:
    static size_t compareSwapCost(const QuerySchema & schema, const SortDefinition  & sort, const int & tuple_cnt);
    static size_t sortCost(const QuerySchema & schema, const SortDefinition  & sort, const int & tuple_cnt);
    static void planCostHelper(const Operator<Bit> *node, size_t & cost) {
        if (!node) return;
        planCostHelper(node->getChild(0), cost);
        planCostHelper(node->getChild(1), cost);
        cost += operatorCost(node);
    }

    static void planPeakMemoryHelper(const Operator<Bit> *node, size_t & peak_memory) {
        if (!node) return;
        planPeakMemoryHelper(node->getChild(0), peak_memory);
        planPeakMemoryHelper(node->getChild(1), peak_memory);
        size_t current_memory = operatorMemory(node);
        peak_memory = std::max(peak_memory, current_memory);
    }


};

} // namespace vaultdb
#endif
