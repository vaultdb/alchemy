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





    B realBin;
    QueryTuple<B> current(input_schema), predecessor(input_schema);

    // TODO: truncating output NYI
    if(output_cardinality_ == 0) { // naive case - go full oblivious
      output_cardinality_ = input->getTupleCount();
    }
    
    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition inputSort = input->getSortOrder();
    SortDefinition outputSort = vector<ColumnSort>(inputSort.begin(), inputSort.begin() + group_by_.size());


    Operator<B>::output_ = shared_ptr<QueryTable<B>>(new QueryTable<B>(input->getTupleCount(), Operator<B>::output_schema_, outputSort));

    QueryTuple<B> tuple = (*Operator<B>::output_)[0];
    B dummy_tag(false);
    tuple.setDummyTag(dummy_tag);


    predecessor = (*input)[0];

    for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
        aggregator->initialize(predecessor, B(false));
    }

    realBin = !predecessor.getDummyTag(); // does this group-by bin contain at least one real (non-dummy) tuple?


    for(uint32_t i = 1; i < input->getTupleCount(); ++i) {
        current = (*input)[i];

        realBin = realBin | !predecessor.getDummyTag();
        B isGroupByMatch = groupByMatch(predecessor, current);

        QueryTuple<B> output_tuple = GroupByAggregate<B>::output_->getTuple(i - 1); // to write to it in place
        generateOutputTuple(output_tuple, predecessor, !isGroupByMatch, realBin, aggregators_);

        for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
            aggregator->initialize(current, isGroupByMatch);
            aggregator->accumulate(current, !isGroupByMatch);
        }

        predecessor = current;
        // reset the flag at the end of each group-by bin
        // flag denotes if we have one non-dummy tuple in the bin
        realBin = Field<B>::If(!isGroupByMatch,B(false), Field<B>(realBin)).template getValue<B>();

    }

    realBin = realBin | !predecessor.getDummyTag();


    // B(true) to make it write out the last entry
    QueryTuple<B> output_tuple = GroupByAggregate<B>::output_->getTuple(input->getTupleCount() - 1);
    generateOutputTuple(output_tuple, predecessor, B(true), realBin, aggregators_);

    // output sorted on group-by cols
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(group_by_.size());
    Operator<B>::output_->setSortOrder(sortDefinition);

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
    QuerySchema outputSchema(group_by_.size() + aggregate_definitions_.size());
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
void GroupByAggregate<B>::generateOutputTuple(QueryTuple<B> &dstTuple, const QueryTuple<B> &lastTuple,
                                                       const B &lastEntryGroupByBin, const B &realBin,
                                                       const vector<GroupByAggregateImpl<B> *> &aggregators) const {

    size_t i;

    // write group-by ordinals
    for(i = 0; i < group_by_.size(); ++i) {
        const Field<B> srcField = lastTuple.getField(group_by_[i]);
        dstTuple.setField(i, srcField);
    }

    // write partial aggs
    for(GroupByAggregateImpl<B> *aggregator : aggregators) {
        Field<B> currentResult = aggregator->getResult();
        dstTuple.setField(i, currentResult);
        ++i;
    }


    B dummyTag = Field<B>::If(lastEntryGroupByBin, Field<B>(!realBin), B(true)).template getValue<B>();
    dstTuple.setDummyTag(dummyTag);


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
