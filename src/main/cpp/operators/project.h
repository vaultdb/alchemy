#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include "operators/expression/expression.h"

typedef std::pair<uint32_t, uint32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping


//template<typename B>
//typedef std::map<uint32_t, Expression<B> > ExpressionMap; // ordinal to expression
typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    template<typename B>
    class Project : public Operator<B> {

        std::vector<ProjectionMapping> projection_map_;
        // TODO: generalize to Expression
        std::map<uint32_t, shared_ptr<Expression<B> > > expressions;

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
            projection_map_.push_back(mapping);
        }

        void addExpression(const shared_ptr<Expression<B> > &expression, const uint32_t &dstOrdinal);

        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const;
    };

}
#endif //_PROJECT_H
