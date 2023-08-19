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
                                                                       group_by_(groupBys)
{
    this->output_cardinality_ = output_card;
    setup();
}


template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const int output_card) : Operator<B>(child, SortDefinition()),
                                                                     aggregate_definitions_(aggregates),
                                                                     group_by_(groupBys) {


    this->output_cardinality_ = output_card;
    setup();
}

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const int output_card, const SortDefinition & effective_sort, const map<int32_t, std::set<int32_t>> &functional_dependency) : Operator<B>(child, SortDefinition()),
                                                                     aggregate_definitions_(aggregates),
                                                                     group_by_(groupBys), effective_sort_(effective_sort), functional_dependency_(functional_dependency) {


    this->output_cardinality_ = output_card;
    setup();
}

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const SortDefinition &sort,
                                            const int & output_card) : Operator<B>(child, sort),
                                                                       aggregate_definitions_(aggregates),
                                                                       group_by_(groupBys) {

    this->output_cardinality_ = output_card;
    setup();
}

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                            const vector<ScalarAggregateDefinition> &aggregates,
                                            const int & output_card) : Operator<B>(child, SortDefinition()),
                                                                       aggregate_definitions_(aggregates),
                                                                       group_by_(groupBys) {

    this->output_cardinality_ = output_card;

    setup();
}


template<typename B>
QueryTable<B> *NestedLoopAggregate<B>::runSelf() {
    QueryTable<B> *input = this->getChild(0)->getOutput();
    QuerySchema input_schema = input->getSchema();
    QuerySchema output_schema = this->output_schema_;

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    this->output_ = TableFactory<B>::getTable(this->output_cardinality_, Operator<B>::output_schema_, input->storageModel());
    QueryTable<B> *output = this->output_;

    // one per aggregator, one per output bin
    vector<vector<UnsortedAggregateImpl<B> *> > per_tuple_aggregators;

    for (int i = 0; i < this->output_cardinality_; ++i) {

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
        B matched = input_dummy;// already "matched" if dummy

        for (int j = 0; j < this->output_cardinality_; ++j) {
            B group_by_match = groupByMatch(input, i, output, j);

            B output_dummy = output->getDummyTag(j);

            // if output is dummy and no match so far, then initialize group-by cols
            B initialize_group_by = output_dummy & !matched;

            for (int k = 0; k < group_by_.size(); ++k) {
                Field<B> src = input->getPackedField(i, group_by_[k]);
                Field<B> dst = Field<B>::If(initialize_group_by, src, output->getPackedField(j, k));
                output->setPackedField(j, k, dst);
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

    return this->output_;

}



template<typename B>
B NestedLoopAggregate<B>::groupByMatch(const QueryTable<B> *src, const int & src_row, const QueryTable<B> *dst, const int & dst_row) const {

    B result = (src->getPackedField(src_row,group_by_[0]) == dst->getPackedField(dst_row,0));
    size_t cursor = 1;

    while(cursor < group_by_.size()) {
        result = result &
                 (src->getPackedField(src_row, group_by_[cursor]) == dst->getPackedField(dst_row, cursor));
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
    bool bit_packing = SystemConfiguration::getInstance().bitPackingEnabled();
    int input_row_cnt = this->getChild(0)->getOutputCardinality();

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
        if((agg.type == AggregateId::MIN || agg.type == AggregateId::MAX) && std::is_same_v<B, Bit> && bit_packing) {
            f = QueryFieldDesc(input_schema.getField(agg.ordinal), i + group_by_.size()); // copy out bit packing info
        }
        else if (agg.type == AggregateId::COUNT&& std::is_same_v<B, Bit> && bit_packing) {
            f = QueryFieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
            f.initializeFieldSizeWithCardinality(input_row_cnt);
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
    QuerySchema input_schema = this->getChild(0)->getOutputSchema();
    int max_cnt = this->getChild(0)->getOutputCardinality();
    int output_ordinal = group_by_.size();


    if(this->output_cardinality_ == 0) { // naive case - go full oblivious
        this->output_cardinality_ = this->getChild(0)->getOutputCardinality();
    }

    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType agg_val_type = (agg.ordinal >= 0) ?
                                 input_schema.getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        UnsortedAggregateImpl<B> *a = (agg.type == AggregateId::AVG)
                                      ?  (UnsortedAggregateImpl<B> *) new UnsortedAvgImpl<B>(AggregateId::AVG, agg_val_type, agg.ordinal, output_ordinal)
                                      :  (UnsortedAggregateImpl<B> *) new UnsortedStatelessAggregateImpl<B>(agg.type, agg_val_type, agg.ordinal, output_ordinal, max_cnt);

        aggregators_.push_back(a);
        ++output_ordinal;
    }



    Operator<B>::output_schema_ = generateOutputSchema(input_schema);

}





template class vaultdb::NestedLoopAggregate<bool>;
template class vaultdb::NestedLoopAggregate<emp::Bit>;