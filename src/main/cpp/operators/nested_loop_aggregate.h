#ifndef _NESTED_LOOP_AGGREGATE_H
#define _NESTED_LOOP_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/unsorted_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class NestedLoopAggregate : public Operator<B> {

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;

        vector<shared_ptr<UnsortedAggregateImpl<B> >> aggregators_;
         int output_cardinality_ = 0;

        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);
        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
			       const vector<ScalarAggregateDefinition> &aggregates, const int output_card);

        NestedLoopAggregate(shared_ptr<QueryTable<B> > child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);

        NestedLoopAggregate(shared_ptr<QueryTable<B> > child, const vector<int32_t> &groupBys,
                         const vector<ScalarAggregateDefinition> &aggregates, const int & output_card = 0);
        ~NestedLoopAggregate() = default;

    protected:
        string OperatorType;
        std::shared_ptr<QueryTable<B> > runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;

    private:


        // returns boolean for whether two tuples are in the same group-by bin
        B groupByMatch(const QueryTuple<B> & lhs, const QueryTuple<B> & rhs) const;

        QuerySchema generateOutputSchema(const QuerySchema & srcSchema) const;

        void setup();



    };
}

#endif //_NESTED_LOOP_AGGREGATE_H
