#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "group_by_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>



using namespace vaultdb;
using namespace std;

template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort,
                                      const size_t & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card)
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
GroupByAggregate<B>::GroupByAggregate(shared_ptr<QueryTable<B>> child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort,
                                      const size_t & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card) {

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(shared_ptr<QueryTable<B>> child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const size_t & output_card) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys) {

    setup();
}


template<typename B>
shared_ptr<QueryTable<B> > GroupByAggregate<B>::runSelf() {
    shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();
    shared_ptr<QuerySchema> input_schema = input->getSchema();

    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType aggValueType = (agg.ordinal >= 0) ?
                                 input_schema->getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);
        aggregators_.push_back(aggregateFactory(agg.type, agg.ordinal, aggValueType));
    }





    //B realBin;
    //QueryTuple<B> current(input_schema), predecessor(input_schema);

    if(output_cardinality_ == 0) { // naive case - go full oblivious
        output_cardinality_ = input->getTupleCount();
    }

    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition inputSort = input->getSortOrder();
    SortDefinition outputSort = vector<ColumnSort>(inputSort.begin(), inputSort.begin() + group_by_.size());


    Operator<B>::output_ = shared_ptr<QueryTable<B>>(new QueryTable<B>(input->getTupleCount(), Operator<B>::output_schema_, outputSort));
    QueryTable<B> *output = Operator<B>::output_.get(); // shorthand
    // SMA: if all dummies at the end, this would be simpler.  But we can't really do that if there are MPC joins, filters, etc before this op because they will sprinkle dummies throughout the table
    for(int j = 0; j < group_by_.size(); ++j) {
        output->assignField(0, j, input.get(), 0, group_by_[j]); //memcpy the input values
    }

    int cursor = group_by_.size();

    // for first pass only
    for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
        aggregator->initialize(input.get()); // don't need group_by_match, only for first pass
        output->setField(0, cursor, aggregator->getResult());
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
            agg->accumulate(input.get(), i, matched);
            output->setField(i, cursor, agg->getResult());
            ++cursor;
        }
        B out_dummy_tag = output->getDummyTag(i-1) & input_dummy_tag; // both need to be dummies for current cell to remain dummy
        output->setDummyTag(i, out_dummy_tag);
        // for output[i-1]
        // if matched, replace previous with current, set old dummy tag to true
        out_dummy_tag = FieldUtilities::select(matched, true_lit, output->getDummyTag(i-1));
        output->setDummyTag(i-1, out_dummy_tag);
    }

    //realBin = !predecessor.getDummyTag(); // does this group-by bin contain at least one real (non-dummy) tuple?


//    for(uint32_t i = 1; i < input->getTupleCount(); ++i) {
//        current = (*input)[i];
//
//        realBin = realBin | !predecessor.getDummyTag();
//        B isGroupByMatch = groupByMatch(predecessor, current);
//
//        QueryTuple<B> output_tuple = GroupByAggregate<B>::output_->getTuple(i - 1); // to write to it in place
//        generateOutputTuple(output_tuple, predecessor, !isGroupByMatch, realBin, aggregators_);
//
//        for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
//            aggregator->initialize(current, isGroupByMatch);// TODO: combine these methods since they are always called together
//            aggregator->accumulate(current, !isGroupByMatch);
//        }
//
//        predecessor = current;
//        // reset the flag at the end of each group-by bin
//        // flag denotes if we have one non-dummy tuple in the bin
//        realBin = Field<B>::If(!isGroupByMatch,B(false), Field<B>(realBin)).template getValue<B>();
//
//    }
//
//    realBin = realBin | !predecessor.getDummyTag();
//
//
//    // B(true) to make it write out the last entry
//    QueryTuple<B> output_tuple = GroupByAggregate<B>::output_->getTuple(input->getTupleCount() - 1);
//    generateOutputTuple(output_tuple, predecessor, B(true), realBin, aggregators_);

    // output sorted on group-by cols
    Operator<B>::output_->setSortOrder(DataUtilities::getDefaultSortDefinition(group_by_.size()));

    for(size_t i = 0; i < aggregators_.size(); ++i) {
        delete aggregators_[i];
    }


    return Operator<B>::output_;

}

template<typename B>
GroupByAggregateImpl<B> *GroupByAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const int32_t &ordinal,
                                                               const FieldType &aggregateValueType) const {

    switch (aggregateType) {
        case AggregateId::COUNT:
            return new GroupByCountImpl<B>(ordinal, aggregateValueType);
        case AggregateId::SUM:
            return new GroupBySumImpl<B>(ordinal, aggregateValueType);
        case AggregateId::AVG:
            return new GroupByAvgImpl<B>(ordinal, aggregateValueType);
        case AggregateId::MIN:
            return new GroupByMinImpl<B>(ordinal, aggregateValueType);
        case AggregateId::MAX:
            return new GroupByMaxImpl<B>(ordinal, aggregateValueType);
        default:
            throw;
    };
}

template<typename B>
bool GroupByAggregate<B>::verifySortOrder(const shared_ptr<QueryTable<B> > &table) const {
    SortDefinition sortedOn = table->getSortOrder();

    return sortCompatible(sortedOn, group_by_);
}

template<typename B>
B GroupByAggregate<B>::groupByMatch(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs) const {

    B result = (lhs.getField(group_by_[0]) == rhs.getField(group_by_[0]));
    size_t cursor = 1;

    while(cursor < group_by_.size()) {
        result = result &
                 (lhs.getField(group_by_[cursor]) == rhs.getField(group_by_[cursor]));
        ++cursor;
    }

    return result;
}

template<typename B>
QuerySchema GroupByAggregate<B>::generateOutputSchema(const QuerySchema & input_schema) const {
    QuerySchema outputSchema;
    size_t i;

    for(i = 0; i < group_by_.size(); ++i) {
        QueryFieldDesc srcField = input_schema.getField(group_by_[i]);
        QueryFieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        dstField.setStringLength(srcField.getStringLength());
        outputSchema.putField(dstField);
    }


    for(i = 0; i < aggregate_definitions_.size(); ++i) {
        ScalarAggregateDefinition agg = aggregate_definitions_[i];
        FieldType agg_type = (agg.ordinal >= 0) ?
                             input_schema.getField(agg.ordinal).getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        QueryFieldDesc fieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
        outputSchema.putField(fieldDesc);
    }

    outputSchema.initializeFieldOffsets();
    return outputSchema;

}

template<typename B>
void GroupByAggregate<B>::generateOutputTuple(QueryTuple<B> &dst_tuple, const QueryTuple<B> &last_tuple,
                                              const B &last_entry_group_by_bin, const B &real_bin,
                                              const vector<GroupByAggregateImpl<B> *> &aggregators) const {

    size_t i;
    // write group-by ordinals
    for(i = 0; i < group_by_.size(); ++i) {
        const Field<B> src_field = last_tuple.getField(group_by_[i]);
        dst_tuple.setField(i, src_field);
    }

    // write partial aggs
    for(GroupByAggregateImpl<B> *aggregator : aggregators) {
        Field<B> current_result = aggregator->getResult();
        dst_tuple.setField(i, current_result);
        ++i;
    }


    B dummy_tag = Field<B>::If(last_entry_group_by_bin, Field<B>(!real_bin), B(true)).template getValue<B>();
    dst_tuple.setDummyTag(dummy_tag);


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