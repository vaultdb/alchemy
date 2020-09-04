//
// Created by Jennie Rogers on 9/2/20.
//

#include "project.h"

Project::Project(const ProjectionDefinition &aDefinition, std::shared_ptr<Operator> &child) : Operator(child), srcSchema(0), dstSchema(aDefinition.size()) {

    projectionDefinition = aDefinition;

    colCount = projectionDefinition.size();

    for(uint32_t i = 0; i < colCount; ++i) {
        // verify that all ordinals are defined
        assert(projectionDefinition.find(i) != projectionDefinition.end());
    }


}

std::shared_ptr<QueryTable> Project::runSelf() {
    std::shared_ptr<QueryTable> srcTable = children[0]->getOutput();
    srcSchema = srcTable->getSchema();
    dstSchema = QuerySchema(colCount); // re-initialize it
    uint32_t tupleCount = srcTable->getTupleCount();

    for(uint32_t ordinal = 0; ordinal < colCount; ++ordinal) {
        QueryFieldDesc aField = getFieldDesc(ordinal, projectionDefinition.at(ordinal));
        dstSchema.appendField(aField);
    }


    output = std::shared_ptr<QueryTable>(new QueryTable(tupleCount, colCount, srcTable->isEncrypted()));
    output->setSchema(dstSchema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        QueryTuple *srcTuple = srcTable->getTuplePtr(i);
        QueryTuple dstTuple = getTuple(srcTuple);
        output->putTuple(i, dstTuple);
    }

    return output;
}

QueryFieldDesc Project::getFieldDesc(const uint32_t & ordinal, const ColumnProjection &aProjection) {

    // it's an expression
    if(aProjection.which() == 0) {
        Expression expression = boost::get<Expression>(aProjection);
        types::TypeId type = expression.getType();
        bool isPrivate = expression.isPrivate();
        std::string alias = expression.getAlias();

        return QueryFieldDesc(ordinal, isPrivate, alias, "unknown", type);

    }

    ProjectionMapping mapping = boost::get<ProjectionMapping>(aProjection);
    uint32_t srcOrdinal = mapping.first;
    uint32_t dstOrdinal = mapping.second;
    const QueryFieldDesc srcField = srcSchema.getField(srcOrdinal);
    return QueryFieldDesc(srcField, dstOrdinal);
}

QueryTuple Project::getTuple(QueryTuple * const srcTuple) const {
    QueryTuple dstTuple(colCount, children[0]->getOutput()->isEncrypted());

    for(uint32_t i = 0; i < colCount; ++i) {
        dstTuple.putField(i, getField(srcTuple, i, projectionDefinition.at(i)));
    }

    return dstTuple;


}

QueryField
Project::getField(QueryTuple *const srcTuple, const uint32_t &ordinal, const ColumnProjection &aProjection) const {

    if(aProjection.which() == 0) {
        Expression expression = boost::get<Expression>(aProjection);
        types::Value fieldValue = expression.expressionCall(*srcTuple);
        return QueryField(ordinal, fieldValue);

    }
    ProjectionMapping mapping = boost::get<ProjectionMapping>(aProjection);
    uint32_t srcOrdinal = mapping.first;
    uint32_t dstOrdinal = mapping.second;

    return QueryField(dstOrdinal, srcTuple->getField(srcOrdinal).getValue());
}

