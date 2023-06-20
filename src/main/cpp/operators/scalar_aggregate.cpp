#include <operators/support/scalar_aggregate_impl.h>
#include "scalar_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/table_factory.h"
#include "util/system_configuration.h"

using namespace vaultdb;

template<typename B>
ScalarAggregate<B>::ScalarAggregate(Operator<B> *child, const vector<ScalarAggregateDefinition> &aggregates,
                                    const SortDefinition &sort)
        : Operator<B>(child, sort), aggregate_definitions_(aggregates) {
    setup();
}

template<typename B>
ScalarAggregate<B>::ScalarAggregate(QueryTable<B> *child,
                                    const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition &sort)
        : Operator<B>(child, sort), aggregate_definitions_(aggregates) {
    setup();
}


template<typename B>
QueryTable<B> *ScalarAggregate<B>::runSelf() {
    QueryTable<B> *input = this->getChild(0)->getOutput();
    QuerySchema input_schema = input->getSchema();
    QuerySchema output_schema = this->output_schema_;


    // use input card to determine how many bits we need for count
    // only needed in secure mode
    if(std::is_same_v<B, Bit> && SystemConfiguration::getInstance().bitPackingEnabled()) {
        for (int i = 0; i < aggregate_definitions_.size(); ++i) {
            auto agg = aggregate_definitions_[i];

            if (agg.type == AggregateId::COUNT) {
                QueryFieldDesc f = this->output_schema_.getField(i);
                f.initializeFieldSizeWithCardinality(input->getTupleCount());
                this->output_schema_.putField(f);
                ((ScalarStatelessAggregateImpl<B> *) aggregators_[i])->bit_packed_size_ = f.size() + 1; // +1 for sign bit
            }
        }
        this->output_schema_.initializeFieldOffsets();
    }

    this->output_ = TableFactory<B>::getTable(1, Operator<B>::output_schema_, input->storageModel());
    QueryTable<B> *output = this->output_;

    for(size_t i = 0; i < input->getTupleCount(); ++i) {
        B input_dummy = input->getDummyTag(i);

        for(ScalarAggregateImpl<B> *aggregator : aggregators_) {
            aggregator->update(input, i, output);
        }
    }

//    for(size_t i = 0; i < aggregators_.size(); ++i) {
//        Field f = aggregators_[i]->getResult();
//        Operator<B>::output_->setField(0, i, f);
//    }

    // dummy tag is always false in our setting, e.g., if we count a set of nulls/dummies, then our count is zero_ - not dummy
    output->setDummyTag(0, false);

    return output;
//    QueryTable<B> *input = Operator<B>::getChild()->getOutput();
//
//    this->start_time_ = clock_start();
//    this->start_gate_cnt_ = emp::CircuitExecution::circ_exec->num_and();
//
//    Operator<B>::output_ = TableFactory<B>::getTable(1, Operator<B>::output_schema_, input->storageModel());
//
//
//    for(size_t i = 0; i < input->getTupleCount(); ++i) {
//        for(ScalarAggregateImpl<B> *aggregator : aggregators_) {
//            aggregator->accumulate(input, i);
//        }
//
//    }
//
//    for(size_t i = 0; i < aggregators_.size(); ++i) {
//        Field f = aggregators_[i]->getResult();
//        Operator<B>::output_->setField(0, i, f);
//    }
//
//    // dummy tag is always false in our setting, e.g., if we count a set of nulls/dummies, then our count is zero_ - not dummy
//    Operator<B>::output_->setDummyTag(0, false);
//
//
//    return this->output_;
}


//template<typename B>
//ScalarAggregateImpl<B> * ScalarAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
//                                                              const QueryFieldDesc &def) const {
//    switch (aggregateType) {
//        case AggregateId::COUNT:
//            return  new ScalarCountImpl<B>(ordinal, def);
//        case AggregateId::SUM:
//            return new ScalarSumImpl<B>(ordinal, def);
//        case AggregateId::AVG:
//            return new ScalarAvgImpl<B>(ordinal, def);
//        case AggregateId::MIN:
//            return new ScalarMinImpl<B>(ordinal, def);
//        case AggregateId::MAX:
//            return new ScalarMaxImpl<B>(ordinal, def);
//        default:
//            throw std::invalid_argument("Not yet implemented!");
//    };
//}

template<typename B>
void  ScalarAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();

    int output_ordinal = 0;
    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType agg_val_type = (agg.ordinal >= 0) ?
                                 input_schema.getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);
        ScalarAggregateImpl<B> *a = (agg.type == AggregateId::AVG)
                                      ?  (ScalarAggregateImpl<B> *) new ScalarAvgImpl<B>(AggregateId::AVG, agg_val_type, agg.ordinal, output_ordinal)
        :  (ScalarAggregateImpl<B> *) new ScalarStatelessAggregateImpl<B>(agg.type, agg_val_type, agg.ordinal, output_ordinal);

        aggregators_.push_back(a);
        output_ordinal++;
    }




    // max and min have the same type as their source column
    // avg, count, sum need to be "stock" types like int32 at least for now
//    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
//
//        aggregators_.push_back(aggregateFactory(agg.type, agg.ordinal, input_schema.getField(agg.ordinal)));
//    }


    this->output_schema_ = QuerySchema(); // reset it

    // generate output schema
    for(size_t i = 0; i < aggregators_.size(); ++i) {
        QueryFieldDesc field_desc(i, aggregate_definitions_[i].alias, "", aggregators_[i]->field_type_, 0);

        if(aggregate_definitions_[i].type == AggregateId::MIN
           || aggregate_definitions_[i].type == AggregateId::MAX) { // carry over definition from source type
            field_desc = QueryFieldDesc(input_schema.getField(aggregate_definitions_[i].ordinal), i);
            field_desc.setName("", aggregate_definitions_[i].alias);

        }
        this->output_schema_.putField(field_desc);
    }
    this->output_schema_.initializeFieldOffsets();
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

