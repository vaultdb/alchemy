#ifndef _NESTED_LOOP_AGGREGATE_H
#define _NESTED_LOOP_AGGREGATE_H


#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include <operators/support/unsorted_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class NestedLoopAggregate : public Operator<B> {

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;

        vector<UnsortedAggregateImpl<B> *> aggregators_;

        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);
        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const int output_card = 0);

        NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);

        NestedLoopAggregate(QueryTable<B>  *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const int & output_card = 0);
        ~NestedLoopAggregate(){
            //if(predicate_ != nullptr) delete predicate_;
        }
        Expression<B> *getPredicate() const { return predicate_; }

    protected:
        string OperatorType;
        Expression<B>  *predicate_;

        QueryTable<B> *runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;

    private:


        // returns boolean for whether two tuples are in the same group-by bin
        B groupByMatch(const QueryTable<B> *src, const int & src_row, const QueryTable<B> *dst, const int & dst_row)  const;

        QuerySchema generateOutputSchema(const QuerySchema & srcSchema) const;

        void setup();



    };
}

#endif //_NESTED_LOOP_AGGREGATE_H