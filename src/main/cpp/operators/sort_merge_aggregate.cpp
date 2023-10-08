#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "sort_merge_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/table_factory.h"
#include "util/system_configuration.h"


using namespace vaultdb;
using namespace std;


template<typename B>
QueryTable<B> *SortMergeAggregate<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();
    QuerySchema input_schema = input->getSchema();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    int output_cursor = this->group_by_.size();
    for (ScalarAggregateDefinition agg: this->aggregate_definitions_) {
        GroupByAggregateImpl<B> *agg_impl = this->aggregateFactory(agg.type, agg.ordinal,
                                                                   input_schema.getField(agg.ordinal));

        // if an aggregator operates on packed bits (e.g. min/max/count), then copy its output definition from source
        if (std::is_same_v<Bit, B> && SystemConfiguration::getInstance().bitPackingEnabled()) {
            if ((agg.type == AggregateId::MIN || agg.type == AggregateId::MAX)){
                QueryFieldDesc packed_field(input_schema.getField(agg.ordinal), output_cursor);
                packed_field.setName("", agg.alias);

                this->output_schema_.putField(packed_field);
            }

            if (agg.type == AggregateId::COUNT) {
                QueryFieldDesc packed_field(output_cursor, agg.alias, "", FieldType::SECURE_LONG);
                packed_field.initializeFieldSizeWithCardinality(input->getTupleCount());
                this->output_schema_.putField(packed_field);
            }

        }

        aggregators_.push_back(agg_impl);
        ++output_cursor;
    }
    this->setOutputCardinality(input->getTupleCount());

    this->output_ = TableFactory<B>::getTable(input->getTupleCount(), this->output_schema_, this->sort_definition_);
    QueryTable<B> *output = this->output_; // shorthand
    // SMA: if all dummies at the end, this would be simpler.  But we can't do that if there are MPC joins, filters, etc before this op because they will sprinkle dummies throughout the table
    for(int j = 0; j < this->group_by_.size(); ++j) {
        output->assignField(0, j, input, 0, this->group_by_[j]); //memcpy the input values
    }

    int cursor = this->group_by_.size();


    // for first row only
    for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
        aggregator->initialize(input); // don't need group_by_match, only for first pass
        output->setPackedField(0, cursor, aggregator->getResult());
        ++cursor;
    }

    output->setDummyTag(0, input->getDummyTag(0));
    B true_lit = true;
    B matched, input_dummy_tag;


    for(int i = 1; i < input->getTupleCount(); ++i) {
        matched = true;
        input_dummy_tag = input->getDummyTag(i);
        for(int j = 0; j < this->group_by_.size(); ++j) {
            matched = matched & (input->getPackedField(i, this->group_by_[j]) == output->getPackedField(i-1, j));
            // initialize output - if input is dummy, copy from predecessor, otherwise copy from input
            Field<B> dst_group_by = Field<B>::If(input_dummy_tag, output->getPackedField(i-1, j), input->getPackedField(i, this->group_by_[j]));
            output->setPackedField(i, j, dst_group_by);
        }

        //  if uninitialized (seen no non-dummies yet), don't create a new group-by bin
        //  if input a dummy also leave group-by bin boundaries as-is
        matched = matched |  output->getDummyTag(i-1) | input_dummy_tag;

        cursor = this->group_by_.size();

        for(auto agg : aggregators_) {
            agg->accumulate(input, i, matched);
            output->setPackedField(i, cursor, agg->getResult());
            ++cursor;
        }
        B out_dummy_tag = output->getDummyTag(i-1) & input_dummy_tag; // both need to be dummies for current cell to remain dummy
        output->setDummyTag(i, out_dummy_tag);
        // for output[i-1]
        // if matched, replace previous with current, set old dummy tag to true
        out_dummy_tag = FieldUtilities::select(matched, true_lit, output->getDummyTag(i-1));
        output->setDummyTag(i-1, out_dummy_tag);
    }



    return this->output_;

}


template<typename B>
void SortMergeAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();

    this->output_schema_ = this->generateOutputSchema(input_schema);
    this->setOutputCardinality(this->getChild()->getOutputCardinality());

    if(!is_optimized_cloned_)
        updateCollation();
}


template class vaultdb::SortMergeAggregate<bool>;
template class vaultdb::SortMergeAggregate<emp::Bit>;