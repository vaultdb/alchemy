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
        static size_t sortMergeJoinCost(SortMergeJoin<Bit> *join);
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


    private:
        static size_t compareSwapCost(const QuerySchema & schema, const SortDefinition  & sort, const int & tuple_cnt);
        static size_t sortCost(const QuerySchema & schema, const SortDefinition  & sort, const int & tuple_cnt);

        };

}
#endif
