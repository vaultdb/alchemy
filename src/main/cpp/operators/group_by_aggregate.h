#ifndef _GROUP_BY_AGGREGATE_H
#define _GROUP_BY_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/group_by_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    class GroupByAggregate : public Operator {

        std::vector<ScalarAggregateDefinition> aggregateDefinitions;
        std::vector<int32_t> groupByOrdinals;

    public:
        GroupByAggregate(std::shared_ptr<Operator> &child, const std::vector<int32_t> &groupBys,
                         const std::vector<ScalarAggregateDefinition> &aggregates) : Operator(child),
                         aggregateDefinitions(aggregates),
                         groupByOrdinals(groupBys) {};


        std::shared_ptr<QueryTable> runSelf() override;

    private:
        GroupByAggregateImpl *aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                               const types::TypeId &aggregateValueType) const;
        // checks that input table is sorted by group-by cols
        bool verifySortOrder(const std::shared_ptr<QueryTable> & table) const;

        // returns boolean for whether two tuples are in the same group-by bin
        types::Value groupByMatch(const QueryTuple & lhs, const QueryTuple & rhs) const;

        QuerySchema generateOutputSchema(const QuerySchema & srcSchema,
                                         const std::vector<GroupByAggregateImpl *> & aggregators) const;

        QueryTuple generateOutputTuple(const QueryTuple &lastTuple, const types::Value &lastEntryGroupByBin,
                            const types::Value &nonDummyBin, const vector<GroupByAggregateImpl *> &aggregators) const;


    };
}

#endif //_GROUP_BY_AGGREGATE_H
