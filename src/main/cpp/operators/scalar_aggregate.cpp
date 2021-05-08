#include <operators/support/scalar_aggregate_impl.h>
#include "scalar_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>


using namespace vaultdb;

template<typename B>
ScalarAggregate<B>::ScalarAggregate(Operator<B> *child, const vector<ScalarAggregateDefinition> &aggregates,
                                    const SortDefinition &sort)
        : Operator<B>(child, sort), aggregate_definitions_(aggregates) {
            setup();

        }

template<typename B>
ScalarAggregate<B>::ScalarAggregate(shared_ptr<QueryTable<B>> child,
                                    const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition &sort)
        : Operator<B>(child, sort), aggregate_definitions_(aggregates) {
            setup();
        }


template<typename B>
std::shared_ptr<QueryTable<B> > ScalarAggregate<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = ScalarAggregate<B>::children_[0]->getOutput();
    QueryTuple<B> tuple(*input->getSchema());
    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(
            new QueryTable<B>(1, Operator<B>::output_schema_, SortDefinition()));

    QueryTuple<B> outputTuple = Operator<B>::output_->getTuple(0);


    for(size_t i = 0; i < input->getTupleCount(); ++i) {
        tuple = input->getTuple(i);
        for(ScalarAggregateImpl<B> *aggregator : aggregators_) {
            aggregator->accumulate(tuple);
        }

    }

    for(size_t i = 0; i < aggregators_.size(); ++i) {
        Field f = aggregators_[i]->getResult();
        outputTuple.setField(i, f);
    }

    Operator<B>::output_->putTuple(0, outputTuple);

    // dummy tag is always false in our setting, e.g., if we count a set of nulls/dummies, then our count is zero - not dummy
    tuple.setDummyTag(false);
    return Operator<B>::output_;
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

    template<typename B>
void  ScalarAggregate<B>::setup() {

    QuerySchema input_schema = Operator<B>::getChild()->getOutputSchema();

    for(ScalarAggregateDefinition agg : aggregate_definitions_) {

        // -1 ordinal for COUNT(*)
        FieldType aggValueType = (agg.ordinal == -1) ?
                                 std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG :
                                 input_schema.getField(agg.ordinal).getType();
        aggregators_.push_back(aggregateFactory(agg.type, agg.ordinal, aggValueType));
    }

        // generate output schema
        Operator<B>::output_schema_ = QuerySchema(aggregators_.size());

        for(size_t i = 0; i < aggregators_.size(); ++i) {
            QueryFieldDesc fieldDesc(i, aggregate_definitions_[i].alias, "", aggregators_[i]->getType());
            Operator<B>::output_schema_.putField(fieldDesc);
        }



    }

template<typename B>
string ScalarAggregate<B>::getOperatorType() const {
    return "ScalarAggregate";
}

template<typename B>
string ScalarAggregate<B>::getParameters() const {
    stringstream ss;
    ss << "aggs: (" << aggregate_definitions_[0].toString();

    for(uint32_t i = 1; i < aggregate_definitions_.size(); ++i) {
        ss << ", " << aggregate_definitions_[i].toString();
    }

    ss << ")";
    return ss.str();
}


template class vaultdb::ScalarAggregate<bool>;
template class vaultdb::ScalarAggregate<emp::Bit>;


