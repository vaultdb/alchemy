#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include "operators/support/expression.h"

typedef std::pair<uint32_t, uint32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping

//JR: variant too messy with inheritance for expression, perhaps debug this later
//typedef boost::variant<Expression, ProjectionMapping> ColumnProjection;

typedef std::map<uint32_t, Expression> ExpressionMap; // ordinal to expression
typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    class Project : public Operator {

        std::vector<ProjectionMapping> projectionMap;
        ExpressionMap expressions;

        uint32_t colCount;
        QuerySchema srcSchema;
        QuerySchema dstSchema;

    public:
        Project(Operator *child);
        Project(shared_ptr<QueryTable> src);

        void addColumnMappings(const ProjectionMappingSet & mapSet);
        void addColumnMapping(const uint32_t &srcOrdinal, const uint32_t &dstOrdinal) {
            ProjectionMapping mapping(srcOrdinal, dstOrdinal);
            projectionMap.push_back(mapping);
        }

        void addExpression(const Expression &expression, const uint32_t &dstOrdinal) {
            expressions[dstOrdinal] = expression;
        }

        std::shared_ptr<QueryTable> runSelf() override;


    private:
        QueryTuple getTuple(QueryTuple *const srcTuple) const;
    };

}
#endif //_PROJECT_H
