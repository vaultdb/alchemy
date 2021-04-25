#include "project.h"
#include <query_table/field/field_factory.h>
#include <query_table/secure_tuple.h>

using namespace vaultdb;

// can't initialize schemas yet, don't have child schema
template<typename B>
Project<B>::Project(Operator<B> *child) : Operator<B>(child), colCount(0), srcSchema(0), dstSchema(0){


}

template<typename B>
Project<B>::Project(shared_ptr<QueryTable<B> > child) : Operator<B>(child), colCount(0), srcSchema(0), dstSchema(0){


}


template<typename B>
std::shared_ptr<QueryTable<B> > Project<B>::runSelf() {

    std::shared_ptr<QueryTable<B> > src_table = Operator<B>::children[0]->getOutput();

    SortDefinition srcSortOrder = src_table->getSortOrder();
    srcSchema = *src_table->getSchema();
    colCount = expressions.size() + projection_map_.size();

    dstSchema = QuerySchema(colCount); // re-initialize it
    uint32_t tupleCount = src_table->getTupleCount();

    // put all of the fields into one data structure -- doubles as a verification that destination schema is fully specified
    std::vector<uint32_t> fieldOrdinals;
    fieldOrdinals.reserve(colCount);

    for(ProjectionMapping mapping : projection_map_) {
        uint32_t dstOrdinal = mapping.second;
        QueryFieldDesc srcField = srcSchema.getField(mapping.first);
        QueryFieldDesc fieldDesc(srcField, dstOrdinal);
        dstSchema.putField(fieldDesc);

        fieldOrdinals.push_back(dstOrdinal);

    }

    auto exprPos = expressions.begin();
    while(exprPos != expressions.end()) {
        uint32_t dstOrdinal = exprPos->first;
        Expression<B> *expression = exprPos->second.get();

        FieldType type = expression->getType();
        std::string alias = expression->getAlias();

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
        for(ProjectionMapping mapping : projection_map_) {
            if(mapping.first == srcOrdinal) {
                dstSortDefinition.push_back(ColumnSort (mapping.second, columnSort.second));
                found = true;
            }
        } // end search for mapping
        if(!found) { sortCarryOver = false; }
    }

    // *** Done defining schema and verifying setup

    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(tupleCount, dstSchema));
    if(sortCarryOver) { Operator<B>::output->setSortOrder(dstSortDefinition);  }


    for(uint32_t i = 0; i < tupleCount; ++i) {
        QueryTuple<B> src_tuple = src_table->getTuple(i);
        QueryTuple<B> dst_tuple = Operator<B>::output->getTuple(i);
        project_tuple(dst_tuple, src_tuple);
    }

    return Operator<B>::output;
}


template<typename B>
void Project<B>::project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const {
    dst_tuple.setDummyTag(src_tuple.getDummyTag());

   auto exprPos = expressions.begin();


    // do all 1:1 mappings
    for(ProjectionMapping mapping : projection_map_) {
        uint32_t srcOrdinal = mapping.first;
        uint32_t dstOrdinal = mapping.second;
        const Field<B> dst_field = src_tuple.getField(srcOrdinal);
        dst_tuple.setField(dstOrdinal, dst_field);

    }

    // exec all expressions
    while(exprPos != expressions.end()) {
        uint32_t dst_ordinal = exprPos->first;
        Expression<B> *expression = exprPos->second.get();
        Field<B> field_value = expression->expressionCall(src_tuple);
        dst_tuple.setField(dst_ordinal, field_value);

        ++exprPos;
    }


}
template<typename B>
void Project<B>::addColumnMappings(const ProjectionMappingSet &mapSet) {
    for(ProjectionMapping mapping: mapSet)
    {
        projection_map_.push_back(mapping);
    }

}

template<typename B>
void Project<B>::addExpression(const shared_ptr<Expression<B>> &expression, const uint32_t &dstOrdinal) {
    expressions[dstOrdinal] = expression;
}


template class vaultdb::Project<bool>;
template class vaultdb::Project<emp::Bit>;
