#ifndef _SCALAR_AGGREGATE_H
#define _SCALAR_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/scalar_aggregate_impl.h>
#include "operator.h"


namespace vaultdb {
    template<typename B>
    class ScalarAggregate : public Operator<B> {
    public:
        // aggregates are sorted by their output order in aggregate's output schema
        ScalarAggregate(Operator<B> *child, const std::vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort = SortDefinition())
                : Operator<B>(child, sort), aggregateDefinitions(aggregates) {};

        ScalarAggregate(shared_ptr<QueryTable<B> > child, const std::vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort = SortDefinition())
                : Operator<B>(child, sort), aggregateDefinitions(aggregates) {};
        ~ScalarAggregate() = default;

        std::shared_ptr<QueryTable<B> > runSelf() override;

    private:
        std::vector<ScalarAggregateDefinition> aggregateDefinitions;

        ScalarAggregateImpl <B> * aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                   const FieldType &aggregateValueType) const;

    };

}
#endif //_SCALAR_AGGREGATE_H
