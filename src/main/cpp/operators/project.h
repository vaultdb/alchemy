#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include <expression/expression.h>

typedef std::pair<uint32_t, uint32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping


//template<typename B>
//typedef std::map<uint32_t, Expression<B> > ExpressionMap; // ordinal to expression
typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    template<typename B>
    class Project : public Operator<B> {

        //std::vector<ProjectionMapping> projection_map_;
        std::map<uint32_t, shared_ptr<Expression<B> > > expressions_; // key = dst_idx, value is expression to evaluate

        //uint32_t col_count_;
        //QuerySchema src_schema_;
        //QuerySchema dst_schema_;
        ProjectionMappingSet column_mappings_;

    public:
        Project(Operator<B> *child, const SortDefinition & sort_definition = SortDefinition());
        Project(shared_ptr<QueryTable<B> > src, const SortDefinition & sort_definition = SortDefinition());
        Project(Operator<B> *child, std::map<uint32_t, shared_ptr<Expression<B> > > expression_map, const SortDefinition & sort_definition = SortDefinition());
        Project(shared_ptr<QueryTable<B> > src, std::map<uint32_t, shared_ptr<Expression<B> > > expression_map, const SortDefinition & sort_definition = SortDefinition());
        ~Project() = default;

        void addColumnMappings(const ProjectionMappingSet & mapSet);
        void addColumnMapping(const uint32_t &src_ordinal, const uint32_t &dst_ordinal);

        void addExpression(const shared_ptr<Expression<B> > &expression, const uint32_t &dstOrdinal);
        void addInputReference(const uint32_t & src_ordinal, const uint32_t & dst_ordinal);

        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const;
    };

}
#endif //_PROJECT_H
