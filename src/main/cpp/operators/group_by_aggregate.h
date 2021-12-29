#ifndef _GROUP_BY_AGGREGATE_H
#define _GROUP_BY_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/group_by_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class GroupByAggregate : public Operator<B> {

        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;

        vector<GroupByAggregateImpl<B> *> aggregators_;
      // truncated output not yet implemented.  Placeholder member variable below
         size_t output_cardinality_ = 0; 

    public:
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const size_t & output_card = 0);
              GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
			       const vector<ScalarAggregateDefinition> &aggregates, const size_t & output_card = 0);

        GroupByAggregate(shared_ptr<QueryTable<B> > child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const size_t & output_card = 0);

        GroupByAggregate(shared_ptr<QueryTable<B> > child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const size_t & output_card = 0);
        ~GroupByAggregate() = default;
        static bool sortCompatible(const SortDefinition & lhs, const vector<int32_t> &group_by_idxs);

    protected:
        std::shared_ptr<QueryTable<B> > runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;

    private:
        GroupByAggregateImpl<B> *aggregateFactory(const AggregateId &aggregateType, const int32_t &ordinal,
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

        void setup();


    };
}

#endif //_GROUP_BY_AGGREGATE_H
