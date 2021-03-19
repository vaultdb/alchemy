#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include "group_by_aggregate.h"
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
std::shared_ptr<QueryTable> GroupByAggregate<B>::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    std::vector<GroupByAggregator<B> *> aggregators;
    QueryTuple *current, *predecessor;
    B realBin;
    FieldType boolType = input->isEncrypted() ? FieldType::SECURE_BOOL : FieldType::BOOL;
    QueryTuple outputTuple;
    QuerySchema inputSchema = input->getSchema();

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType aggValueType = (agg.ordinal >= 0) ?
                    inputSchema.getField(agg.ordinal).getType() :
                                     (input->isEncrypted() ? FieldType::SECURE_LONG : FieldType::LONG);
        aggregators.push_back(aggregateFactory(agg.type, agg.ordinal, aggValueType));
    }


    // sorted on group-by cols
    assert(verifySortOrder(input));

    QuerySchema outputSchema = generateOutputSchema(inputSchema, aggregators);

    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition inputSort = input->getSortOrder();
    SortDefinition outputSort = std::vector<ColumnSort>(inputSort.begin(), inputSort.begin() + groupByOrdinals.size());


    output = std::shared_ptr<QueryTable>(new QueryTable(input->getTupleCount(), outputSchema, outputSort));

    // TODO: should this set dummy tag to input->getTuple(0).getDummyTag?
    QueryTuple *tuplePtr = output->getTuplePtr(0);
    B dummyTag(false);
    tuplePtr->setDummyTag(dummyTag);


    predecessor = input->getTuplePtr(0);;
    B forceInit = static_cast<B>(FieldFactory::getZero(boolType));

    for(GroupByAggregator<B> *aggregator : aggregators) {
        aggregator->initialize(*predecessor, forceInit);
    }

    B previousDummyTag = static_cast<const B &>(*predecessor->getDummyTag());
    realBin = !previousDummyTag; // does this group-by bin contain at least one real (non-dummy) tuple?


    for(uint32_t i = 1; i < input->getTupleCount(); ++i) {
        current = input->getTuplePtr(i);
        previousDummyTag = static_cast<const B &>(*predecessor->getDummyTag());
        realBin = realBin | !previousDummyTag;
        B isGroupByMatch = groupByMatch(*predecessor, *current);
        outputTuple = generateOutputTuple(*predecessor, !isGroupByMatch, realBin, aggregators);

        for(GroupByAggregator<B> *aggregator : aggregators) {
            aggregator->initialize(*current, isGroupByMatch);
            aggregator->accumulate(*current, !isGroupByMatch);
        }

        output->putTuple(i-1, outputTuple);

        predecessor = current;
        // reset the flag that denotes if we have one non-dummy tuple in the bin
        GroupByAggregator<B> * agg = aggregators[0];
        agg->updateGroupByBinBoundary(!isGroupByMatch, realBin);
    }

    previousDummyTag = static_cast<const B &>(*predecessor->getDummyTag());
    realBin = realBin | !previousDummyTag;

    // getOne to make it write out the last entry
    outputTuple = generateOutputTuple(*predecessor, FieldFactory::getOne(boolType), realBin, aggregators);
    output->putTuple(input->getTupleCount() - 1, outputTuple);

    // output sorted on group-by cols
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(groupByOrdinals.size());
    output->setSortOrder(sortDefinition);

     for(size_t i = 0; i < aggregators.size(); ++i) {
        delete aggregators[i];
    }

    return output;

}

template<typename B>
GroupByAggregator<B> *GroupByAggregate<B>::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                         const FieldType &fieldType) const {

    bool isEncrypted = TypeUtilities::isEncrypted(fieldType);
    if (!isEncrypted) {
        switch (aggregateType) {
            case AggregateId::COUNT:
               // GroupByCountImpl<LongField, BoolField> counter(ordinal, fieldType);
                //GroupByAggregator<B> *out = static_cast<GroupByAggregator<B *>();
                return new GroupByCountImpl<LongField, B>(ordinal, fieldType);
        }
    }

/*            case AggregateId::SUM:
                return new GroupBySumImpl(ordinal, fieldType);
            case AggregateId::AVG:
                return new GroupByAvgImpl(ordinal, fieldType);
            case AggregateId::MIN:
                return new GroupByMinImpl(ordinal, fieldType);
            case AggregateId::MAX:
                return new GroupByMaxImpl(ordinal, fieldType);
        };
    }

    switch (aggregateType) {
        case AggregateId::COUNT:
            return new SecureGroupByCountImpl(ordinal, aggregateValueType);
        case AggregateId::SUM:
            return new SecureGroupBySumImpl(ordinal, aggregateValueType);
        case AggregateId::AVG:
            return new SecureGroupByAvgImpl(ordinal, aggregateValueType);
        case AggregateId::MIN:
            return new SecureGroupByMinImpl(ordinal, aggregateValueType);
        case AggregateId::MAX:
            return new SecureGroupByMaxImpl(ordinal, aggregateValueType);

    };*/
    return nullptr;
}

template<typename B>
bool GroupByAggregate<B>::verifySortOrder(const std::shared_ptr<QueryTable> &table) const {
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
B GroupByAggregate<B>::groupByMatch(const QueryTuple &lhs, const QueryTuple &rhs) const {

    Field *eq = FieldUtilities::equal(lhs.getField(groupByOrdinals[0]), rhs.getField(groupByOrdinals[0]));
    B result = static_cast<B&>(*eq);
    delete eq;

    size_t cursor = 1;
    while(cursor < groupByOrdinals.size()) {
        eq = FieldUtilities::equal(lhs.getField(groupByOrdinals[0]), rhs.getField(groupByOrdinals[0]));
        result = result & static_cast<B&>(*eq);
        delete eq;
        ++cursor;
    }

    return result;
}

template<typename B>
QuerySchema GroupByAggregate<B>::generateOutputSchema(const QuerySchema & srcSchema, const vector<GroupByAggregator<B> *> & aggregators) const {
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
QueryTuple GroupByAggregate<B>::generateOutputTuple(const QueryTuple &lastTuple, const Field &lastEntryGroupByBin,
                                                 const Field &nonDummyBin,
                                                 const vector<GroupByAggregator<B> *> &aggregators) const {

    QueryTuple dstTuple(groupByOrdinals.size() + aggregators.size());
    size_t i;

    // write group-by ordinals
    for(i = 0; i < groupByOrdinals.size(); ++i) {
        const Field *srcField = lastTuple.getField(groupByOrdinals[i]);
        dstTuple.putField(i, *srcField);
    }

    // write partial aggs
    for(GroupByAggregator<B> *aggregator : aggregators) {
        Field currentResult = aggregator->getResult();
        dstTuple.putField(i, currentResult);
        ++i;
    }


    B dummyTag = aggregators[0]->getDummyTag(lastEntryGroupByBin, nonDummyBin);
    dstTuple.setDummyTag(dummyTag);


    return dstTuple;

}



template class vaultdb::GroupByAggregate<BoolField>;
template class vaultdb::GroupByAggregate<SecureBoolField>;
