#include <operators/support/scalar_aggregate_impl.h>
#include "scalar_aggregate.h"

using namespace vaultdb;

template<typename B>
std::shared_ptr<QueryTable<B> > ScalarAggregate<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = ScalarAggregate<B>::children[0]->getOutput();
    std::vector<ScalarAggregateImpl<B> *> aggregators;
    QueryTuple<B> tuple;

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {

        // -1 ordinal for COUNT(*)
        FieldType aggValueType = (agg.ordinal == -1) ?
                input->isEncrypted() ? FieldType::SECURE_LONG : FieldType::LONG :
                input->getSchema()->getField(agg.ordinal).getType();
        aggregators.push_back(aggregateFactory(agg.type, agg.ordinal, aggValueType));
    }

    for(size_t i = 0; i < input->getTupleCount(); ++i) {
        tuple = input->getTuple(i);
        for(ScalarAggregateImpl<B> *aggregator : aggregators) {
            aggregator->accumulate(tuple);
        }

    }
    // generate output schema
    QuerySchema outputSchema(aggregators.size());

    for(size_t i = 0; i < aggregators.size(); ++i) {
        QueryFieldDesc fieldDesc(i, aggregateDefinitions[i].alias, "", aggregators[i]->getType());
        outputSchema.putField(fieldDesc);
    }


    Operator<B>::output = std::shared_ptr<QueryTable<B> >(
            new QueryTable<B>(1, outputSchema, SortDefinition()));

    QueryTuple<B> outputTuple = Operator<B>::output->getTuple(0);


    for(size_t i = 0; i < aggregators.size(); ++i) {
        Field f = aggregators[i]->getResult();
        outputTuple.setField(i, f);
    }

    Operator<B>::output->putTuple(0, outputTuple);

    // dummy tag is always false in our setting, e.g., if we count a set of nulls/dummies, then our count is zero - not dummy
    tuple.setDummyTag(false);
    return Operator<B>::output;
}


template<typename B>
ScalarAggregateImpl<B> * ScalarAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                            const FieldType &aggregateValueType) const {
        switch (aggregateType) {
            case AggregateId::COUNT:
                return  new ScalarCountImpl<B>(ordinal, aggregateValueType);
            case AggregateId::SUM:
                return new ScalarSumImpl<B>(ordinal, aggregateValueType);
            case AggregateId::AVG:
                return new ScalarAvgImpl<B>(ordinal, aggregateValueType);
            case AggregateId::MIN:
                return new ScalarMinImpl<B>(ordinal, aggregateValueType);
            case AggregateId::MAX:
                return new ScalarMaxImpl<B>(ordinal, aggregateValueType);
            default:
                throw std::invalid_argument("Not yet implemented!");
        };
    }

template class vaultdb::ScalarAggregate<bool>;
template class vaultdb::ScalarAggregate<emp::Bit>;


