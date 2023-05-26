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

    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(
            new QueryTable<B>(1, Operator<B>::output_schema_, SortDefinition()));

    QueryTuple<B> output_tuple = Operator<B>::output_->getTuple(0);

    for(size_t i = 0; i < input->getTupleCount(); ++i) {
        QueryTuple<B> tuple = input->getTuple(i);
        for(ScalarAggregateImpl<B> *aggregator : aggregators_) {
            aggregator->accumulate(tuple);
        }

    }

    for(size_t i = 0; i < aggregators_.size(); ++i) {
        Field f = aggregators_[i]->getResult();
        output_tuple.setField(i, f);
    }

    Operator<B>::output_->putTuple(0, output_tuple);

    // dummy tag is always false in our setting, e.g., if we count a set of nulls/dummies, then our count is zero_ - not dummy
    output_tuple.setDummyTag(false);

    return Operator<B>::output_;
}


template<typename B>
ScalarAggregateImpl<B> * ScalarAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                              const QueryFieldDesc &def) const {
    switch (aggregateType) {
        case AggregateId::COUNT:
            return  new ScalarCountImpl<B>(ordinal, def);
        case AggregateId::SUM:
            return new ScalarSumImpl<B>(ordinal, def);
        case AggregateId::AVG:
            return new ScalarAvgImpl<B>(ordinal, def);
        case AggregateId::MIN:
            return new ScalarMinImpl<B>(ordinal, def);
        case AggregateId::MAX:
            return new ScalarMaxImpl<B>(ordinal, def);
        default:
            throw std::invalid_argument("Not yet implemented!");
    };
}

template<typename B>
void  ScalarAggregate<B>::setup() {

    QuerySchema input_schema = Operator<B>::getChild()->getOutputSchema();

    // max and min have the same type as their source column
    // avg, count, sum need to be "stock" types like int32 at least for now
    for(ScalarAggregateDefinition agg : aggregate_definitions_) {

        aggregators_.push_back(aggregateFactory(agg.type, agg.ordinal, input_schema.getField(agg.ordinal)));
    }


    Operator<B>::output_schema_ = QuerySchema(); // reset it

    // generate output schema
    for(size_t i = 0; i < aggregators_.size(); ++i) {
        QueryFieldDesc field_desc(i, aggregate_definitions_[i].alias, "", aggregators_[i]->getType());

        if(aggregate_definitions_[i].type == AggregateId::MIN
           || aggregate_definitions_[i].type == AggregateId::MAX) { // carry over definition from source type
            field_desc = QueryFieldDesc(input_schema.getField(aggregate_definitions_[i].ordinal), i);
            field_desc.setName("", aggregate_definitions_[i].alias);

        }
        Operator<B>::output_schema_.putField(field_desc);

    }

    Operator<B>::output_schema_.initializeFieldOffsets();

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

