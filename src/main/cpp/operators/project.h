//
// Created by Jennie Rogers on 9/2/20.
//

#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include "operators/support/expression.h"

typedef std::pair<uint32_t, uint32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping
typedef boost::variant<Expression, ProjectionMapping> ColumnProjection;

typedef std::map<uint32_t, ColumnProjection> ProjectionDefinition;



class Project  : public Operator {

    ProjectionDefinition  projectionDefinition;
    uint32_t colCount;
    QuerySchema srcSchema;
    QuerySchema dstSchema;

public:
    Project(const ProjectionDefinition  & aDefinition, std::shared_ptr<Operator> &child);
    std::shared_ptr<QueryTable> runSelf() override;


private:
    QueryFieldDesc getFieldDesc(const uint32_t & ordinal, const ColumnProjection & aProjection);
    QueryTuple getTuple(QueryTuple * const srcTuple) const;
    QueryField getField(QueryTuple *const srcTuple, const uint32_t &ordinal, const ColumnProjection &aProjection) const;
};


#endif //_PROJECT_H
