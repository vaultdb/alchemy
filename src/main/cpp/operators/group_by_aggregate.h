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
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const size_t & output_card = 0);
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
			       const vector<ScalarAggregateDefinition> &aggregates, const size_t & output_card = 0);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const size_t & output_card = 0);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const size_t & output_card = 0);
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
