#include "project.h"
#include <query_table/field/field_factory.h>


// can't initialize schemas yet, don't have child schema
template<typename B>
Project<B>::Project(Operator<B> *child) : Operator<B>(child), colCount(0), srcSchema(0), dstSchema(0){


}

template<typename B>
Project<B>::Project(shared_ptr<QueryTable<B> > child) : Operator<B>(child), colCount(0), srcSchema(0), dstSchema(0){


}


template<typename B>
std::shared_ptr<QueryTable<B> > Project<B>::runSelf() {

    std::shared_ptr<QueryTable<B> > srcTable = Operator<B>::children[0]->getOutput();
    SortDefinition srcSortOrder = srcTable->getSortOrder();
    srcSchema = srcTable->getSchema();
    colCount = expressions.size() + projectionMap.size();

    dstSchema = QuerySchema(colCount); // re-initialize it
    uint32_t tupleCount = srcTable->getTupleCount();

    // put all of the fields into one data structure -- doubles as a verification that destination schema is fully specified
    std::vector<uint32_t> fieldOrdinals;
    fieldOrdinals.reserve(colCount);

    for(ProjectionMapping mapping : projectionMap) {
        uint32_t dstOrdinal = mapping.second;
        QueryFieldDesc srcField = srcSchema.getField(mapping.first);
        QueryFieldDesc fieldDesc(srcField, dstOrdinal);
        size_t srcStringLength = srcField.getStringLength();
        fieldDesc.setStringLength(srcStringLength);
        dstSchema.putField(fieldDesc);

        fieldOrdinals.push_back(dstOrdinal);

    }

    auto exprPos = expressions.begin();
    while(exprPos != expressions.end()) {
        uint32_t dstOrdinal = exprPos->first;
        Expression expression = exprPos->second;

        FieldType type = expression.getType();
        std::string alias = expression.getAlias();

        QueryFieldDesc fieldDesc = QueryFieldDesc(dstOrdinal, alias, "", type); // NYI: string length for expressions
        dstSchema.putField(fieldDesc);

        fieldOrdinals.push_back(dstOrdinal);
        ++exprPos;
    }

    // confirm that all ordinals are defined
    for(uint32_t i = 0; i < colCount; ++i) {
        assert(std::find(fieldOrdinals.begin(), fieldOrdinals.end(), i) != fieldOrdinals.end());
    }


    // *** Check to see if order-by carries over
    bool sortCarryOver = true;
    SortDefinition  dstSortDefinition;
    for(ColumnSort columnSort : srcSortOrder) {
        uint32_t srcOrdinal = columnSort.first;
        bool found = false;
        for(ProjectionMapping mapping : projectionMap) {
            if(mapping.first == srcOrdinal) {
                dstSortDefinition.push_back(ColumnSort (mapping.second, columnSort.second));
                found = true;
            }
        } // end search for mapping
        if(!found) { sortCarryOver = false; }
    }

    // *** Done defining schema and verifying setup

    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(tupleCount, colCount));
    Operator<B>::output->setSchema(dstSchema);
    if(sortCarryOver) { Operator<B>::output->setSortOrder(dstSortDefinition);  }


    for(uint32_t i = 0; i < tupleCount; ++i) {
        QueryTuple srcTuple = srcTable->getTuplePtr(i);
        QueryTuple dstTuple = getTuple(srcTuple);
        Operator<B>::output->putTuple(i, dstTuple);
    }

    return Operator<B>::output;
}


template<typename B>
QueryTuple<B> Project<B>::getTuple(QueryTuple<B> * const srcTuple) const {
    QueryTuple dstTuple(colCount, Operator<B>::children[0]->getOutput()->isEncrypted());
    dstTuple.setDummyTag(*(srcTuple->getDummyTag()));

   auto exprPos = expressions.begin();


    // do all 1:1 mappings
    for(ProjectionMapping mapping : projectionMap) {
        uint32_t srcOrdinal = mapping.first;
        uint32_t dstOrdinal = mapping.second;
        const Field<B> *dstField = srcTuple->getField(srcOrdinal);
        dstTuple.putField(dstOrdinal, *dstField);

    }

    // exec all expressions
    while(exprPos != expressions.end()) {
        uint32_t dstOrdinal = exprPos->first;
        Expression expression = exprPos->second;
        Field<B> *fieldValue = expression.expressionCall(*srcTuple);
        dstTuple.putField(dstOrdinal, *fieldValue);
        delete fieldValue;

        ++exprPos;
    }

    return dstTuple;


}
template<typename B>
void Project<B>::addColumnMappings(const ProjectionMappingSet &mapSet) {
    for(ProjectionMapping mapping: mapSet)
    {
        projectionMap.push_back(mapping);
    }

}

