#ifndef _GROUP_BY_AGGREGATE_H
#define _GROUP_BY_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/group_by_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class GroupByAggregate : public Operator<B> {

        std::vector<ScalarAggregateDefinition> aggregateDefinitions;
        std::vector<int32_t> groupByOrdinals;

    public:
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates) : Operator<B>(child),
                         aggregateDefinitions(aggregates),
                         groupByOrdinals(groupBys) {};

        GroupByAggregate(shared_ptr<QueryTable<B> > child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates) : Operator<B>(child),
                                                                                     aggregateDefinitions(aggregates),
                                                                                     groupByOrdinals(groupBys) {};
        std::shared_ptr<QueryTable<B> > runSelf() override;
        ~GroupByAggregate() = default;

    private:
        GroupByAggregateImpl<B> *aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                               const FieldType &aggregateValueType) const;
        // checks that input table is sorted by group-by cols
        bool verifySortOrder(const std::shared_ptr<QueryTable<B> > & table) const;

        // returns boolean for whether two tuples are in the same group-by bin
        B groupByMatch(const QueryTuple<B> & lhs, const QueryTuple<B> & rhs) const;

        QuerySchema generateOutputSchema(const QuerySchema & srcSchema,
                                         const std::vector<GroupByAggregateImpl<B> *> & aggregators) const;

        void generateOutputTuple(QueryTuple <B> &dstTuple, const QueryTuple <B> &lastTuple,
                                           const B &lastEntryGroupByBin, const B &real_bin,
                                 const vector<GroupByAggregateImpl<B> *> &aggregators) const;


    };
}

#endif //_GROUP_BY_AGGREGATE_H
