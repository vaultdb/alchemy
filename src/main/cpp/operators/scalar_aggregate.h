#ifndef _SCALAR_AGGREGATE_H
#define _SCALAR_AGGREGATE_H


#include <operators/support/aggregate_id.h>
#include <operators/support/scalar_aggregate_impl.h>
#include "operator.h"

// only serves plaintext inputs for now
// TODO: extend this for secure case
// TODO: handle case where all inputs are dummies?
namespace vaultdb {
    class ScalarAggregate : public Operator {
    public:
        // aggregrates are sorted by their output order in aggregate's output schema
        ScalarAggregate(std::shared_ptr<Operator> &child, const std::vector<ScalarAggregateDefinition> &aggregates)
                : Operator(child), aggregateDefinitions(aggregates) {};

        std::shared_ptr<QueryTable> runSelf() override;

    private:
        std::vector<ScalarAggregateDefinition> aggregateDefinitions;

        ScalarAggregateImpl *aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal, const bool &isEncrypted) const;

    };

}
#endif //_SCALAR_AGGREGATE_H
