#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <operators/support/secure_group_by_aggregate_impl.h>
#include "group_by_aggregate.h"


using namespace vaultdb;

std::shared_ptr<QueryTable> GroupByAggregate::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    std::vector<GroupByAggregateImpl *> aggregators;
    QueryTuple *current, *predecessor;
    Field *realBin;
    types::TypeId boolType = input->isEncrypted() ? types::FieldType::SECURE_BOOL : types::FieldType::BOOL;
    QueryTuple outputTuple;
    QuerySchema inputSchema = input->getSchema();

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        types::TypeId aggValueType = (agg.ordinal >= 0) ?
                    inputSchema.getField(agg.ordinal).getType() :
                                     (input->isEncrypted() ? types::FieldType::SECURE_LONG : types::FieldType::LONG);
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
    types::Value dummyTag = output->isEncrypted() ? types::Value(emp::Bit(false)) : types::Value(false);
    tuplePtr->setDummyTag(dummyTag);


    predecessor = input->getTuplePtr(0);;
    types::Value forceInit = TypeUtilities::getZero(boolType);

    for(GroupByAggregateImpl *aggregator : aggregators) {
        aggregator->initialize(*predecessor, forceInit);
    }

    realBin = !(predecessor->getDummyTag()); // does this group-by bin contain at least one real (non-dummy) tuple?


    for(uint32_t i = 1; i < input->getTupleCount(); ++i) {
        current = input->getTuplePtr(i);
        realBin = realBin | !(predecessor->getDummyTag());
        types::Value isGroupByMatch = groupByMatch(*predecessor, *current);
        outputTuple = generateOutputTuple(*predecessor, !isGroupByMatch, realBin, aggregators);

        for(GroupByAggregateImpl *aggregator : aggregators) {
            aggregator->initialize(*current, isGroupByMatch);
            aggregator->accumulate(*current, !isGroupByMatch);
        }

        output->putTuple(i-1, outputTuple);

        predecessor = current;
        // reset the flag that denotes if we have one non-dummy tuple in the bin
        aggregators[0]->updateGroupByBinBoundary(!isGroupByMatch, realBin);
    }

    realBin = realBin | !predecessor->getDummyTag();

    // getOne to make it write out the last entry
    outputTuple = generateOutputTuple(*predecessor, TypeUtilities::getOne(boolType), realBin, aggregators);
    output->putTuple(input->getTupleCount() - 1, outputTuple);

    // output sorted on group-by cols
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(groupByOrdinals.size());
    output->setSortOrder(sortDefinition);

     for(size_t i = 0; i < aggregators.size(); ++i) {
        delete aggregators[i];
    }

    return output;

}

GroupByAggregateImpl *GroupByAggregate::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                         const types::TypeId &aggregateValueType) const {

    bool isEncrypted = TypeUtilities::isEncrypted(aggregateValueType);
    if (!isEncrypted) {
        switch (aggregateType) {
            case AggregateId::COUNT:
                return new GroupByCountImpl(ordinal, aggregateValueType);
            case AggregateId::SUM:
                return new GroupBySumImpl(ordinal, aggregateValueType);
            case AggregateId::AVG:
                return new GroupByAvgImpl(ordinal, aggregateValueType);
            case AggregateId::MIN:
                return new GroupByMinImpl(ordinal, aggregateValueType);
            case AggregateId::MAX:
                return new GroupByMaxImpl(ordinal, aggregateValueType);
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

    };
    return nullptr;
}

bool GroupByAggregate::verifySortOrder(const std::shared_ptr<QueryTable> &table) const {
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

types::Value GroupByAggregate::groupByMatch(const QueryTuple &lhs, const QueryTuple &rhs) const {

    types::Value result = lhs.getFieldPtr(groupByOrdinals[0])->getValue() ==  rhs.getFieldPtr(groupByOrdinals[0])->getValue();
    size_t cursor = 1;
    while(cursor < groupByOrdinals.size()) {
        result = result &
                (lhs.getFieldPtr(groupByOrdinals[cursor])->getValue() ==  rhs.getFieldPtr(groupByOrdinals[cursor])->getValue());
        ++cursor;
    }

    return result;
}

QuerySchema GroupByAggregate::generateOutputSchema(const QuerySchema & srcSchema, const vector<GroupByAggregateImpl *> & aggregators) const {
    QuerySchema outputSchema(groupByOrdinals.size() + aggregateDefinitions.size());
    size_t i;

    for(i = 0; i < groupByOrdinals.size(); ++i) {
        FieldDesc srcField = srcSchema.getField(groupByOrdinals[i]);
        FieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        dstField.setStringLength(srcField.getStringLength());
        outputSchema.putField(dstField);
    }

    for(i = 0; i < aggregateDefinitions.size(); ++i) {
        FieldDesc fieldDesc(i + groupByOrdinals.size(), aggregateDefinitions[i].alias, "", aggregators[i]->getType());
        outputSchema.putField(fieldDesc);
    }

    return outputSchema;

}

QueryTuple GroupByAggregate::generateOutputTuple(const QueryTuple &lastTuple, const types::Value &lastEntryGroupByBin,
                                                 const types::Value &nonDummyBin,
                                                 const vector<GroupByAggregateImpl *> &aggregators) const {

    QueryTuple dstTuple(groupByOrdinals.size() + aggregators.size());
    size_t i;

    // write group-by ordinals
    for(i = 0; i < groupByOrdinals.size(); ++i) {
        Field *srcField = lastTuple.getFieldPtr(groupByOrdinals[i]);
        Field dstField(i, srcField->getValue());
        dstTuple.putField(dstField, -1);
    }

    // write partial aggs
    for(GroupByAggregateImpl *aggregator : aggregators) {
        types::Value currentResult = aggregator->getResult() const;
        Field dstField(i, currentResult);
        dstTuple.putField(dstField, -1);
        ++i;
    }


    types::Value dummyTag = aggregators[0]->getDummyTag(lastEntryGroupByBin, nonDummyBin);
    dstTuple.setDummyTag(dummyTag);


    return dstTuple;

}

