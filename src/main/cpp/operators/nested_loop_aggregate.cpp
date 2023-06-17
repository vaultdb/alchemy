#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "nested_loop_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/table_factory.h"
#include "util/system_configuration.h"


using namespace vaultdb;
using namespace std;

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const SortDefinition &sort,
                                            const int & output_card) : Operator<B>(child, sort),
                                                                       aggregate_definitions_(aggregates),
                                                                       group_by_(groupBys), output_cardinality_(output_card)
{
    setup();
}


template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const int output_card) : Operator<B>(child, SortDefinition()),
                                                                     aggregate_definitions_(aggregates),
                                                                     group_by_(groupBys), output_cardinality_(output_card) {
    output_cardinality_ = output_card;
    setup();
}

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const SortDefinition &sort,
                                            const int & output_card) : Operator<B>(child, sort),
                                                                       aggregate_definitions_(aggregates),
                                                                       group_by_(groupBys), output_cardinality_(output_card) {

    setup();
}

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const int & output_card) : Operator<B>(child, SortDefinition()),
                                                                       aggregate_definitions_(aggregates),
                                                                       group_by_(groupBys) {

    setup();
}


template<typename B>
QueryTable<B> *NestedLoopAggregate<B>::runSelf() {
    QueryTable<B> *input = this->getChild(0)->getOutput();
    QuerySchema input_schema = input->getSchema();
    QuerySchema output_schema = this->output_schema_;


    // use input card to determine how many bits we need for count
    // only needed in secure mode
    if(std::is_same_v<B, Bit> && SystemConfiguration::getInstance().bitPackingEnabled()) {
        for (int i = 0; i < aggregate_definitions_.size(); ++i) {
            auto agg = aggregate_definitions_[i];

            if (agg.type == AggregateId::COUNT) {
                QueryFieldDesc f = this->output_schema_.getField(group_by_.size() + i);
                f.initializeFieldSizeWithCardinality(input->getTupleCount());
                this->output_schema_.putField(f);
                ((UnsortedStatelessAggregateImpl<B> *) aggregators_[i])->bit_packed_size_ = f.size() + 1; // +1 for sign bit
            }
        }
        this->output_schema_.initializeFieldOffsets();
    }

    if(output_cardinality_ == 0) { // naive case - go full oblivious
        output_cardinality_ = input->getTupleCount();
    }

    this->output_ = TableFactory<B>::getTable(input->getTupleCount(), Operator<B>::output_schema_, input->storageModel());
    QueryTable<B> *output = this->output_;

    // one per aggregator, one per output bin
    vector<vector<UnsortedAggregateImpl<B> *> > per_tuple_aggregators;
    //confirm all output dummyTag as true
    for (int i = 0; i < output_cardinality_; ++i) {
        this->output_->setDummyTag(i, true);
        per_tuple_aggregators.emplace_back(vector<UnsortedAggregateImpl<B> *>());
        vector<UnsortedAggregateImpl<B> *> row_aggregators(aggregators_.size());
        for(int j = 0; j < aggregators_.size(); ++j) {
            UnsortedAggregateImpl<B> *a = (aggregators_[j]->agg_type_ == AggregateId::AVG)
                                          ?    new UnsortedAvgImpl<B>(AggregateId::AVG, aggregators_[j]->field_type_, aggregators_[j]->input_ordinal_, aggregators_[j]->output_ordinal_)
                                          :     aggregators_[j];
            per_tuple_aggregators[i].emplace_back(a);
        }
    }



    // create output tuples with managed memory for ease of use
    for(int i = 0; i < input->getTupleCount(); ++i) {

        B input_dummy = input->getDummyTag(i);
        B matched = FieldUtilities::select(input_dummy, B(true), B(false)); // already "matched" if dummy

        for (int j = 0; j < output_cardinality_; ++j) {
            B group_by_match = groupByMatch(input, i, output, j);
            B output_dummy = output->getDummyTag(j);

            // if output is dummy and no match so far, then initialize group-by cols
            B initialize_group_by = output_dummy & !matched;

            for (int k = 0; k < group_by_.size(); ++k) {
                Field<B> src = input->getField(i, group_by_[k]);
                Field<B> dst = Field<B>::If(initialize_group_by, src, output->getField(j, k));
                output->setField(j, k, dst);
            }

            for (int k = 0; k < aggregators_.size(); ++k) {
                UnsortedAggregateImpl<B>  *a = per_tuple_aggregators[j][k];
                a->update(input, i, output, j,matched, group_by_match);
            } // end aggregators


            // if group-by match or output_dummy, then matched = true
            matched = FieldUtilities::select(group_by_match | initialize_group_by, B(true), matched);
            output_dummy =  FieldUtilities::select(initialize_group_by, B(false), output_dummy);
            output->setDummyTag(j, output_dummy);

        } // end for each  output tuple
    }// end for each input tuple

    for(int i = 0; i < aggregate_definitions_.size(); ++i) {
            delete aggregators_[i];
        if(aggregate_definitions_[i].type == AggregateId::AVG) {
            for(int j = 0; j < per_tuple_aggregators.size(); ++j) {
                delete per_tuple_aggregators[j][i];
            }
        }
    }

    return Operator<B>::output_;

}



template<typename B>
B NestedLoopAggregate<B>::groupByMatch(const QueryTable<B> *src, const int & src_row, const QueryTable<B> *dst, const int & dst_row) const {

    B result = (src->getField(src_row,group_by_[0]) == dst->getField(dst_row,0));
    size_t cursor = 1;

    while(cursor < group_by_.size()) {
        result = result &
                 (src->getField(src_row, group_by_[cursor]) == dst->getField(dst_row, cursor));
        ++cursor;
    }

    // if one or both inputs are dummies, then set to NOT matched
    result = FieldUtilities::select(src->getDummyTag(src_row) | dst->getDummyTag(dst_row), B(false), result);
    return result;
}


template<typename B>
QuerySchema NestedLoopAggregate<B>::generateOutputSchema(const QuerySchema & input_schema) const {
    QuerySchema output_schema;
    size_t i;

    for(i = 0; i < group_by_.size(); ++i) {
        QueryFieldDesc srcField = input_schema.getField(group_by_[i]);
        QueryFieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        dstField.setStringLength(srcField.getStringLength());
        output_schema.putField(dstField);
    }


    for(i = 0; i < aggregate_definitions_.size(); ++i) {
        ScalarAggregateDefinition agg = aggregate_definitions_[i];
        FieldType agg_type = (agg.ordinal >= 0) ?
                             input_schema.getField(agg.ordinal).getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);
        QueryFieldDesc f;
        if(agg.type == AggregateId::MIN || agg.type == AggregateId::MAX && std::is_same_v<B, Bit>) {
            f = QueryFieldDesc(input_schema.getField(agg.ordinal), i + group_by_.size()); // copy out bit packing info
        }
        else {
            f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
        }
        output_schema.putField(f);
    }

    output_schema.initializeFieldOffsets();
    return output_schema;

}

template<typename B>
void NestedLoopAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();
    int output_ordinal = group_by_.size();

    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType agg_val_type = (agg.ordinal >= 0) ?
                                 input_schema.getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        UnsortedAggregateImpl<B> *a = (agg.type == AggregateId::AVG)
                                      ?  (UnsortedAggregateImpl<B> *) new UnsortedAvgImpl<B>(AggregateId::AVG, agg_val_type, agg.ordinal, output_ordinal)
                                      :  (UnsortedAggregateImpl<B> *) new UnsortedStatelessAggregateImpl<B>(agg.type, agg_val_type, agg.ordinal, output_ordinal);

        aggregators_.push_back(a);
        ++output_ordinal;
    }



    Operator<B>::output_schema_ = generateOutputSchema(input_schema);

}

template<typename B>
string NestedLoopAggregate<B>::getOperatorType() const {
    return "NestedLoopAggregate";
}

template<typename B>
string NestedLoopAggregate<B>::getParameters() const {
    stringstream  ss;
    ss << "group-by: (" << group_by_[0];
    for(uint32_t i = 1; i < group_by_.size(); ++i)
        ss << ", " << group_by_[i];

    ss << ") aggs: (" << aggregate_definitions_[0].toString();

    for(uint32_t i = 1; i < aggregate_definitions_.size(); ++i) {
        ss << ", " << aggregate_definitions_[i].toString();
    }

    ss << ")";
    return ss.str();
}



template class vaultdb::NestedLoopAggregate<bool>;
template class vaultdb::NestedLoopAggregate<emp::Bit>;