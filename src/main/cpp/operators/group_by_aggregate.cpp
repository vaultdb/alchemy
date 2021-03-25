#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "group_by_aggregate.h"


using namespace vaultdb;

template<typename B>
std::shared_ptr<QueryTable<B> > GroupByAggregate<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children[0]->getOutput();
    std::vector<GroupByAggregateImpl<B> *> aggregators;
    QueryTuple<B> *current, *predecessor;
    B realBin;

    QueryTuple<B> outputTuple;
    QuerySchema inputSchema = input->getSchema();

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType aggValueType = (agg.ordinal >= 0) ?
                                     inputSchema.getField(agg.ordinal).getType() :
                                     (input->isEncrypted() ? FieldType::SECURE_LONG :  FieldType::LONG);
        aggregators.push_back(aggregateFactory(agg.type, agg.ordinal, aggValueType));
    }


    // sorted on group-by cols
    assert(verifySortOrder(input));

    QuerySchema outputSchema = generateOutputSchema(inputSchema, aggregators);

    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition inputSort = input->getSortOrder();
    SortDefinition outputSort = std::vector<ColumnSort>(inputSort.begin(), inputSort.begin() + groupByOrdinals.size());


    Operator<B>::output = std::shared_ptr<QueryTable<B>>(new QueryTable<B>(input->getTupleCount(), outputSchema, outputSort));

    QueryTuple<B> *tuplePtr = Operator<B>::output->getTuplePtr(0);
    tuplePtr->setDummyTag(B(false));


    predecessor = input->getTuplePtr(0);;

    for(GroupByAggregateImpl<B> *aggregator : aggregators) {
        aggregator->initialize(*predecessor, B(false));
    }

    realBin = !(*predecessor->getDummyTag()); // does this group-by bin contain at least one real (non-dummy) tuple?


    for(uint32_t i = 1; i < input->getTupleCount(); ++i) {
        current = input->getTuplePtr(i);

        realBin = realBin | !(*predecessor->getDummyTag());
        B isGroupByMatch = groupByMatch(*predecessor, *current);
        outputTuple = generateOutputTuple(*predecessor, !isGroupByMatch, realBin, aggregators);

        for(GroupByAggregateImpl<B> *aggregator : aggregators) {
            aggregator->initialize(*current, isGroupByMatch);
            aggregator->accumulate(*current, !isGroupByMatch);
        }

        Operator<B>::output->putTuple(i-1, outputTuple);
        predecessor = current;
        // reset the flag at the end of each group-by bin
        // flag denotes if we have one non-dummy tuple in the bin
        realBin = (B) Field<B>::If(!isGroupByMatch,B(false), realBin);

    }

    realBin = realBin | !(*(predecessor->getDummyTag()));


    // B(true) to make it write out the last entry
    outputTuple = generateOutputTuple(*predecessor, B(true), realBin, aggregators);
    Operator<B>::output->putTuple(input->getTupleCount() - 1, outputTuple);

    // output sorted on group-by cols
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(groupByOrdinals.size());
    Operator<B>::output->setSortOrder(sortDefinition);

    for(size_t i = 0; i < aggregators.size(); ++i) {
        delete aggregators[i];
    }

    return Operator<B>::output;

}

template<typename B>
GroupByAggregateImpl<B> *GroupByAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
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
bool GroupByAggregate<B>::verifySortOrder(const std::shared_ptr<QueryTable<B> > &table) const {
    SortDefinition sortedOn = table->getSortOrder();
    assert(sortedOn.size() >= groupByOrdinals.size());

    for(size_t idx = 0; idx < groupByOrdinals.size(); ++idx) {
        // ASC || DESC does not matter here
        if(groupByOrdinals[idx] != sortedOn[idx].first) {
            return false;
        }
    }

    return true;

}

template<typename B>
B GroupByAggregate<B>::groupByMatch(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs) const {

    B result = *lhs.getField(groupByOrdinals[0]) ==  *rhs.getField(groupByOrdinals[0]);
    size_t cursor = 1;

    while(cursor < groupByOrdinals.size()) {
        result = result &
                 (*lhs.getField(groupByOrdinals[cursor]) ==  *rhs.getField(groupByOrdinals[cursor]));
        ++cursor;
    }

    return result;
}

template<typename B>
QuerySchema GroupByAggregate<B>::generateOutputSchema(const QuerySchema & srcSchema, const vector<GroupByAggregateImpl<B> *> & aggregators) const {
    QuerySchema outputSchema(groupByOrdinals.size() + aggregateDefinitions.size());
    size_t i;

    for(i = 0; i < groupByOrdinals.size(); ++i) {
        QueryFieldDesc srcField = srcSchema.getField(groupByOrdinals[i]);
        QueryFieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        dstField.setStringLength(srcField.getStringLength());
        outputSchema.putField(dstField);
    }

    for(i = 0; i < aggregateDefinitions.size(); ++i) {
        QueryFieldDesc fieldDesc(i + groupByOrdinals.size(), aggregateDefinitions[i].alias, "", aggregators[i]->getType());
        outputSchema.putField(fieldDesc);
    }

    return outputSchema;

}

template<typename B>
QueryTuple<B> GroupByAggregate<B>::generateOutputTuple(const QueryTuple<B> &lastTuple, const B &lastEntryGroupByBin,
                                                 const B &realBin,
                                                 const vector<GroupByAggregateImpl<B> *> &aggregators) const {

    QueryTuple<B> dstTuple(groupByOrdinals.size() + aggregators.size());
    size_t i;

    // write group-by ordinals
    for(i = 0; i < groupByOrdinals.size(); ++i) {
        const Field<B> *srcField = lastTuple.getField(groupByOrdinals[i]);
        dstTuple.putField(i, *srcField);
    }

    // write partial aggs
    for(GroupByAggregateImpl<B> *aggregator : aggregators) {
        Field<B> currentResult = aggregator->getResult();
        dstTuple.putField(i, currentResult);
        ++i;
    }


    B dummyTag = (B) Field<B>::If(lastEntryGroupByBin, !realBin, B(true));
    dstTuple.setDummyTag(dummyTag);


    return dstTuple;

}


template class vaultdb::GroupByAggregate<BoolField>;
template class vaultdb::GroupByAggregate<SecureBoolField>;