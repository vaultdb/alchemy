#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include <expression/expression.h>

typedef std::pair<int32_t, int32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping


//template<typename B>
//typedef std::map<uint32_t, Expression<B> > ExpressionMap; // ordinal to expression
typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    template<typename B>
    class Project : public Operator<B> {

        std::map<uint32_t, Expression<B> * > expressions_; // key = dst_idx, value is expression to evaluate
        ProjectionMappingSet column_mappings_;
        vector<uint32_t> exprs_to_exec_;

    public:
        Project(Operator<B> *child, std::map<uint32_t, Expression<B> * > expression_map, const SortDefinition & sort_definition = SortDefinition()) : Operator<B>(child, sort_definition), expressions_(expression_map) {

            setup();
        }

        Project(QueryTable<B> *child, std::map<uint32_t, Expression<B> * > expression_map, const SortDefinition & sort= SortDefinition()) : Operator<B>(child, sort), expressions_(expression_map) {
            setup();

        }
        ~Project() {
            for(auto pos : expressions_) {
                if(pos.second != nullptr) delete pos.second;
            }
        }

        QueryTable<B> *runSelf() override;


    private:
      //  void project_tuple(QueryTuple<B> &dst_tuple, QueryTuple<B> &src_tuple) const;
        void setup();

    protected:
        inline string getOperatorType() const override {     return "Project"; }

        inline string getParameters() const override {
            stringstream ss;

            auto expr_pos = expressions_.begin();
            ss << "(" << "<" << expr_pos->first << ", " << expr_pos->second->toString() << ">";
            ++expr_pos;
            while(expr_pos != expressions_.end()) {
                ss << ", "  << "<" << expr_pos->first << ", " << expr_pos->second->toString() << ">";
                ++expr_pos;
            }

            ss << ")";

            return ss.str();

        }
    };

    // to create projections with simple 1:1 mappings
    template<typename B>
    class ExpressionMapBuilder {
    public:
        ExpressionMapBuilder(const QuerySchema & input_schema);

        void addMapping(const uint32_t & src_idx, const uint32_t & dst_idx);
        void addExpression( Expression<B> * expression, const uint32_t & dst_idx );

        std::map<uint32_t, Expression<B> * > getExprs() const { return expressions_; }

    private:
        std::map<uint32_t, Expression<B> *> expressions_;
        QuerySchema src_schema_;


    };


}
#endif //_PROJECT_H
