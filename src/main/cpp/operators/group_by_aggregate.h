#ifndef _GROUP_BY_AGGREGATE_H
#define _GROUP_BY_AGGREGATE_H

#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include <operators/support/group_by_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class GroupByAggregate : public Operator<B> {

        vector<GroupByAggregateImpl<B> *> aggregators_;
      // truncated output not yet implemented.  Placeholder member variable below
         size_t output_cardinality_ = 0; 

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;
        bool check_sort_ = true;

        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort);
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
			       const vector<ScalarAggregateDefinition> &aggregates);

        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates);

        Operator<B> *clone() const override {
            return new GroupByAggregate<B>(this->lhs_child_->clone(), this->group_by_, this->aggregate_definitions_, this->check_sort_);
        }

        virtual ~GroupByAggregate()  {
            for(size_t i = 0; i < aggregators_.size(); ++i) {
                delete aggregators_[i];
            }
        }
        static bool sortCompatible(const SortDefinition & lhs, const vector<int32_t> &group_by_idxs);

    protected:
        QueryTable<B> *runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;

    private:
        GroupByAggregateImpl<B> *aggregateFactory(const AggregateId &aggregator_type, const int32_t &ordinal,
                                                  const QueryFieldDesc &input_schema) const;


        QuerySchema generateOutputSchema(const QuerySchema & srcSchema) const;


        void setup();


    };
}

#endif //_GROUP_BY_AGGREGATE_H
