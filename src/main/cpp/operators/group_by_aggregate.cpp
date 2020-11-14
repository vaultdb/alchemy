#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "group_by_aggregate.h"


std::shared_ptr<QueryTable> GroupByAggregate::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    std::vector<GroupByAggregateImpl *> aggregators;
    QueryTuple *current, *predecessor;
    types::Value nonDummyBin;
    QueryTuple outputTuple;

    for(ScalarAggregateDefinition agg : aggregateDefinitions) {
        aggregators.push_back(aggregateFactory(agg.type, agg.ordinal, input->isEncrypted()));
    }


    // sorted on group-by cols
    assert(verifySortOrder(input));
    QuerySchema outputSchema = generateOutputSchema(input->getSchema(), aggregators);
    // output sort order equal to first group-by-col-count entries in input sort order
    SortDefinition inputSort = input->getSortOrder();
    SortDefinition outputSort = std::vector<ColumnSort>(inputSort.begin(), inputSort.begin() + groupByOrdinals.size());

    std::shared_ptr<QueryTable> output(new QueryTable(input->getTupleCount(), outputSchema.getFieldCount(), input->isEncrypted()));
    output->setSchema(outputSchema);
    output->setSortOrder(outputSort);


    QueryTuple *tuplePtr = output->getTuplePtr(0);
    tuplePtr->initDummy();

    predecessor = input->getTuplePtr(0);
    for(GroupByAggregateImpl *aggregator : aggregators) {
        aggregator->initialize(*predecessor, types::Value(false));
    }

    nonDummyBin = !(predecessor->getDummyTag()); // does this group-by bin contain at least one real (non-dummy) tuple?


    for(int i = 1; i < input->getTupleCount(); ++i) {
        current = input->getTuplePtr(i);
        nonDummyBin = nonDummyBin | !(predecessor->getDummyTag());
        types::Value isGroupByMatch = groupByMatch(*predecessor, *current);
        outputTuple = generateOutputTuple(*predecessor, !isGroupByMatch, nonDummyBin, aggregators);

        for(GroupByAggregateImpl *aggregator : aggregators) {
            aggregator->initialize(*current, isGroupByMatch);
            aggregator->accumulate(*current, !isGroupByMatch);
        }

        output->putTuple(i-1, outputTuple);
        predecessor = current;
        // reset the flag that denotes if we have one non-dummy tuple in the bin
        aggregators[0]->updateGroupByBinBoundary(!isGroupByMatch, nonDummyBin);
    }

    nonDummyBin = nonDummyBin | predecessor->getDummyTag();
    types::TypeId dummyType = nonDummyBin.getType();

    // getOne to make it write out the last entry
    outputTuple = generateOutputTuple(*predecessor, TypeUtilities::getOne(dummyType), nonDummyBin, aggregators);
    output->putTuple(input->getTupleCount() - 1, outputTuple);

    // output sorted on group-by cols
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(groupByOrdinals.size());
    output->setSortOrder(sortDefinition);

    /* TODO: debug this cleanup later
     * for(int i = 0; i < aggregators.size(); ++i) {
        delete aggregators[i];
    }*/

    return output;

}

GroupByAggregateImpl *GroupByAggregate::aggregateFactory(const AggregateId &aggregateType, const uint32_t &ordinal,
                                                         const bool &isEncrypted) const {
    if (!isEncrypted) {
        switch (aggregateType) {
            case AggregateId::COUNT:
                return new GroupByCountImpl(ordinal);
            case AggregateId::SUM:
            case AggregateId::AVG:
            case AggregateId::MIN:
            case AggregateId::MAX:
                throw std::invalid_argument("Not yet implemented!");
        };
    }

    switch (aggregateType) {
        case AggregateId::AVG:
        case AggregateId::COUNT:
        case AggregateId::SUM:
        case AggregateId::MIN:
        case AggregateId::MAX:
            throw std::invalid_argument("Not yet implemented!");
    };
}

bool GroupByAggregate::verifySortOrder(const std::shared_ptr<QueryTable> &table) const {
    SortDefinition sortedOn = table->getSortOrder();
    for(int idx = 0; idx < groupByOrdinals.size(); ++idx) {
        // ASC || DESC does not matter here
        if(groupByOrdinals[idx] != sortedOn[idx].first) {
            return false;
        }
    }

    return true;

}

types::Value GroupByAggregate::groupByMatch(const QueryTuple &lhs, const QueryTuple &rhs) const {
    types::Value result = lhs.getFieldPtr(groupByOrdinals[0])->getValue() ==  rhs.getFieldPtr(groupByOrdinals[0])->getValue();
    int cursor = 1;
    while(cursor < groupByOrdinals.size()) {
        result = result &
                lhs.getFieldPtr(groupByOrdinals[cursor])->getValue() ==  rhs.getFieldPtr(groupByOrdinals[cursor])->getValue();
        ++cursor;
    }

    return result;
}

QuerySchema GroupByAggregate::generateOutputSchema(const QuerySchema & srcSchema, const vector<GroupByAggregateImpl *> & aggregators) const {
    QuerySchema outputSchema(groupByOrdinals.size() + aggregateDefinitions.size());
    int i;

    for(i = 0; i < groupByOrdinals.size(); ++i) {
        QueryFieldDesc srcField = srcSchema.getField(groupByOrdinals[i]);
        QueryFieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        outputSchema.putField(dstField);
    }

    for(i = 0; i < aggregateDefinitions.size(); ++i) {
        QueryFieldDesc fieldDesc(i + groupByOrdinals.size(), aggregateDefinitions[i].alias, "", aggregators[i]->getType());
        outputSchema.putField(fieldDesc);
    }

    return outputSchema;

}

QueryTuple GroupByAggregate::generateOutputTuple(const QueryTuple &lastTuple, const types::Value &lastEntryGroupByBin,
                                                 const types::Value &nonDummyBin,
                                                 const vector<GroupByAggregateImpl *> &aggregators) const {

    QueryTuple dstTuple(groupByOrdinals.size() + aggregators.size());
    int i;

    // write group-by ordinals
    for(i = 0; i < groupByOrdinals.size(); ++i) {
        QueryField *srcField = lastTuple.getFieldPtr(groupByOrdinals[i]);
        QueryField dstField(i, srcField->getValue());
        dstTuple.putField(dstField);
    }

    // write partial aggs
    for(GroupByAggregateImpl *aggregator : aggregators) {
        types::Value currentResult = aggregator->getResult();
        QueryField dstField(i, currentResult);
        dstTuple.putField(dstField);
        ++i;
    }


    types::Value dummyTag = aggregators[0]->getDummyTag(lastEntryGroupByBin, nonDummyBin);
    dstTuple.setDummyTag(dummyTag);

    return dstTuple;

}

