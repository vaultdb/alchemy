#include <operators/support/scalar_aggregate_impl.h>
#include <operators/support/secure_scalar_aggregate_impl.h>
#include "scalar_aggregate.h"

std::shared_ptr<QueryTable> ScalarAggregate::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    std::vector<ScalarAggregateImpl *> aggregators;
    QueryTuple *tuple;

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {
        aggregators.push_back(aggregateFactory(agg.type, agg.ordinal, input->isEncrypted()));
    }

    QueryTuple *first = input->getTuplePtr(0);
    for(ScalarAggregateImpl *aggregator : aggregators) {
        aggregator->initialize(*first);
    }

    for(int i = 1; i < input->getTupleCount(); ++i) {
        tuple = input->getTuplePtr(i);
        for(ScalarAggregateImpl *aggregator : aggregators) {
            aggregator->accumulate(*tuple);
        }
    }


    // generate output schema
    QuerySchema outputSchema(aggregators.size());

    for(int i = 0; i < aggregators.size(); ++i) {
        QueryFieldDesc fieldDesc(i, aggregateDefinitions[i].alias, "", aggregators[i]->getType());
        outputSchema.putField(fieldDesc);
    }

    output = std::shared_ptr<QueryTable>(new QueryTable(1, outputSchema.getFieldCount(), input->isEncrypted()));
    output->setSchema(outputSchema);
    QueryTuple *tuplePtr = output->getTuplePtr(0);
    tuplePtr->initDummy();

    for(int i = 0; i < aggregators.size(); ++i) {
        QueryField field(i, aggregators[i]->getResult());
        tuplePtr->putField(field);
        delete aggregators[i];
    }

    // TODO: handle the case where all input tuples are dummies
    types::Value dummyTag = (output->isEncrypted()) ? types::Value(emp::Bit(false, emp::PUBLIC)) : types::Value(false);
    tuplePtr->setDummyTag(dummyTag);


        return output;
}


// TODO: update aggregate factory with min/max/avg
ScalarAggregateImpl *ScalarAggregate::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                       const bool &isEncrypted) const {
    if (!isEncrypted) {
        switch (aggregateType) {
            case AggregateId::COUNT:
                return new ScalarCount(ordinal);
            case AggregateId::SUM:
                return new ScalarSum(ordinal);
            case AggregateId::AVG:
            case AggregateId::MIN:
            case AggregateId::MAX:
                throw std::invalid_argument("Not yet implemented!");
        };
    }

    switch (aggregateType) {
        case AggregateId::AVG:
            return new SecureScalarAverage(ordinal);
        case AggregateId::COUNT:
        case AggregateId::SUM:
        case AggregateId::MIN:
        case AggregateId::MAX:
            throw std::invalid_argument("Not yet implemented!");
    };
}


