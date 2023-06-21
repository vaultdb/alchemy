#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "group_by_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/table_factory.h"
#include "util/system_configuration.h"


using namespace vaultdb;
using namespace std;

// TODO: implement count with packed bits
template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort,
                                      const size_t & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(group_bys), output_cardinality_(output_card)
{

    setup();
}


template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const size_t & output_card) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card) {

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort,
                                      const size_t & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card) {

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const size_t & output_card) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys) {

    setup();
}


template<typename B>
QueryTable<B> *GroupByAggregate<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();
    QuerySchema input_schema = input->getSchema();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.emp_manager_->andGateCount();

    int output_cursor = group_by_.size();
    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        GroupByAggregateImpl<B> *agg_impl = aggregateFactory(agg.type, agg.ordinal, input_schema.getField(agg.ordinal));

        // if an aggregator operates on packed bits (e.g. min/max/count), then copy its output definition from source
        if(agg_impl->packedFields()) {
            QueryFieldDesc packed_field(input_schema.getField(agg.ordinal), output_cursor);
            packed_field.setName("", agg.alias);

            if (agg.type == AggregateId::COUNT) {
                packed_field.initializeFieldSizeWithCardinality(input->getTupleCount());
                agg_impl->bit_packed_size_ = packed_field.size() + 1; // +1 for sign bit
            }

            this->output_schema_.putField(packed_field);
        }
        aggregators_.push_back(agg_impl);
        ++output_cursor;
    }

    if(output_cardinality_ == 0) { // naive case - go full oblivious
        output_cardinality_ = input->getTupleCount();
    }


    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition input_sort = input->getSortOrder();
    SortDefinition output_sort = vector<ColumnSort>(input_sort.begin(), input_sort.begin() + group_by_.size());

    Operator<B>::output_ = TableFactory<B>::getTable(input->getTupleCount(), Operator<B>::output_schema_, input->storageModel(), output_sort);
    QueryTable<B> *output = Operator<B>::output_; // shorthand
    // SMA: if all dummies at the end, this would be simpler.  But we can't really do that if there are MPC joins, filters, etc before this op because they will sprinkle dummies throughout the table
    for(int j = 0; j < group_by_.size(); ++j) {
        output->assignField(0, j, input, 0, group_by_[j]); //memcpy the input values
    }

    int cursor = group_by_.size();


    // for first row only
    for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
        aggregator->initialize(input); // don't need group_by_match, only for first pass
        output->setField(0, cursor, aggregator->getResult(), aggregator->packedFields());
        ++cursor;
    }

    output->setDummyTag(0, input->getDummyTag(0));
    B true_lit = true;
    B matched, input_dummy_tag;

    for(int i = 1; i < input->getTupleCount(); ++i) {
        matched = true;
        input_dummy_tag = input->getDummyTag(i);
        for(int j = 0; j < group_by_.size(); ++j) {
            matched = matched & (input->getField(i, group_by_[j]) == output->getField(i-1, j));
            // initialize output - if input is dummy, copy from predecessor, otherwise copy from input
            Field<B> dst_group_by = Field<B>::If(input_dummy_tag, output->getField(i-1, j), input->getField(i, group_by_[j]));
            output->setField(i, j, dst_group_by);
        }

        //  if uninitialized (seen no non-dummies yet), don't create a new group-by bin
        //  if input a dummy also leave group-by bin boundaries as-is
        matched = matched |  output->getDummyTag(i-1) | input_dummy_tag;

        cursor = group_by_.size();

        for(auto agg : aggregators_) {
            agg->accumulate(input, i, matched);
            output->setField(i, cursor, agg->getResult(), agg->packedFields());
            ++cursor;
        }
        B out_dummy_tag = output->getDummyTag(i-1) & input_dummy_tag; // both need to be dummies for current cell to remain dummy
        output->setDummyTag(i, out_dummy_tag);
        // for output[i-1]
        // if matched, replace previous with current, set old dummy tag to true
        out_dummy_tag = FieldUtilities::select(matched, true_lit, output->getDummyTag(i-1));
        output->setDummyTag(i-1, out_dummy_tag);
    }


    // output sorted on group-by cols
    Operator<B>::output_->setSortOrder(DataUtilities::getDefaultSortDefinition(group_by_.size()));



    return Operator<B>::output_;

}

template<typename B>
GroupByAggregateImpl<B> *GroupByAggregate<B>::aggregateFactory(const AggregateId &aggregator_type, const int32_t &ordinal,
                                                               const QueryFieldDesc &input_schema) const {

    // for most aggs the output type is the same as the input type
    // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
    FieldType input_type = (ordinal >= 0) ?
                             input_schema.getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

    switch (aggregator_type) {
        case AggregateId::COUNT:
            return new GroupByCountImpl<B>(ordinal, input_type);
        case AggregateId::SUM:
            return new GroupBySumImpl<B>(ordinal, input_type);
        case AggregateId::AVG:
            return new GroupByAvgImpl<B>(ordinal, input_type);
        case AggregateId::MIN:
            return new GroupByMinImpl<B>(ordinal, input_schema);
        case AggregateId::MAX:
            return new GroupByMaxImpl<B>(ordinal, input_schema);
        default:
            throw;
    };
}


template<typename B>
QuerySchema GroupByAggregate<B>::generateOutputSchema(const QuerySchema & input_schema) const {
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

        QueryFieldDesc field_desc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
        output_schema.putField(field_desc);
    }

    output_schema.initializeFieldOffsets();
    return output_schema;

}


template<typename B>
bool GroupByAggregate<B>::sortCompatible(const SortDefinition & sorted_on, const vector<int32_t> &group_by_idxs) {
    if(sorted_on.size() < group_by_idxs.size())
        return false;

    for(size_t idx = 0; idx < group_by_idxs.size(); ++idx) {
        // ASC || DESC does not matter here
        if(group_by_idxs[idx] != sorted_on[idx].first) {
            return false;
        }
    }

    return true;

}

template<typename B>
void GroupByAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();
    SortDefinition input_sort = Operator<B>::getChild(0)->getSortOrder();

    Operator<B>::output_schema_ = generateOutputSchema(input_schema);


    // sorted on group-by cols
    assert(sortCompatible(input_sort, group_by_));


}

template<typename B>
string GroupByAggregate<B>::getOperatorType() const {
    return "GroupByAggregate";
}

template<typename B>
string GroupByAggregate<B>::getParameters() const {
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


template class vaultdb::GroupByAggregate<bool>;
template class vaultdb::GroupByAggregate<emp::Bit>;