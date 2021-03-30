#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include "operators/support/expression.h"

typedef std::pair<uint32_t, uint32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping

//JR: variant too messy with inheritance for expression, perhaps debug this later
//typedef boost::variant<Expression, ProjectionMapping> ColumnProjection;

//template<typename B>
//typedef std::map<uint32_t, Expression<B> > ExpressionMap; // ordinal to expression
typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    template<typename B>
    class Project : public Operator<B> {

        std::vector<ProjectionMapping> projectionMap;
        std::map<uint32_t, Expression<B> > expressions;

        uint32_t colCount;
        QuerySchema srcSchema;
        QuerySchema dstSchema;

    public:
        Project(Operator<B> *child);
        Project(shared_ptr<QueryTable<B> > src);
        ~Project() = default;

        void addColumnMappings(const ProjectionMappingSet & mapSet);
        void addColumnMapping(const uint32_t &srcOrdinal, const uint32_t &dstOrdinal) {
            ProjectionMapping mapping(srcOrdinal, dstOrdinal);
            projectionMap.push_back(mapping);
        }

        void addExpression(const Expression<B> &expression, const uint32_t &dstOrdinal) {
            expressions[dstOrdinal] = expression;
        }

        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const;
    };

}
#endif //_PROJECT_H
