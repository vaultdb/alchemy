#ifndef _PROJECT_H
#define _PROJECT_H

#include "operator.h"
#include "common/defs.h"
#include <expression/expression.h>

typedef std::pair<int32_t, int32_t> ProjectionMapping; // src ordinal, dst ordinal

// single projection, it is either an expression over 2+ fields or it is a 1:1 mapping spec'd in projection mapping

typedef std::vector<ProjectionMapping> ProjectionMappingSet;

namespace vaultdb {
    template<typename B>
    class Project : public Operator<B> {

        map<uint32_t, Expression<B> * > expressions_; // key = dst_idx, value is expression to evaluate
        ProjectionMappingSet column_mappings_;
        vector<uint32_t> exprs_to_exec_;

    public:
        Project(Operator<B> *child, std::map<uint32_t, Expression<B> * > expression_map, const SortDefinition & sort_definition = SortDefinition()) : Operator<B>(child, sort_definition), expressions_(expression_map) {

            setup();
        }

        Project(QueryTable<B> *child, std::map<uint32_t, Expression<B> * > expression_map, const SortDefinition & sort= SortDefinition()) : Operator<B>(child, sort), expressions_(expression_map) {
            setup();

        }

        Project(const Project<B> & src) : Operator<B>(src) {
            for(auto pos : src.expressions_) {
                expressions_[pos.first] = pos.second->clone();
            }

            setup();
        }

        Operator<B> *clone() const override {
            return new Project<B>(*this);
        }

        void updateCollation() override;

        ~Project() {
            for(auto pos : expressions_) {
                if(pos.second != nullptr) delete pos.second;
            }
        }

        QueryTable<B> *runSelf() override;

        std::map<uint32_t, Expression<B> * > getExpressions() const { return expressions_; }
        std::vector<uint32_t> getExpressionsToExec() const { return exprs_to_exec_; }

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::PROJECT) {
                return false;
            }

            const Project<B> &other_node = static_cast<const Project<B> &>(other);
            if (expressions_.size() != other_node.expressions_.size()) {
                return false;
            }

            for (auto pos : expressions_) {
                if (other_node.expressions_.find(pos.first) == other_node.expressions_.end()) {
                    return false;
                }
                if (!(*pos.second == *other_node.expressions_.at(pos.first))) {
                    return false;
                }
            }

            if(this->column_mappings_ != other_node.column_mappings_) return false;
            if(this->exprs_to_exec_ != other_node.exprs_to_exec_) return false;
            return this->operatorEquality(other);
        }

        int getSourceOrdinal(const int & output_ordinal) const override {

            for (ProjectionMapping mapping: column_mappings_) {
                if (mapping.second == output_ordinal) {
                    return mapping.first;
                }
            }
            throw runtime_error("Output ordinal " + std::to_string(output_ordinal) + " has no 1:1 mapping in source relation!");
        }

    private:
        void setup();

    protected:
        inline OperatorType getType() const override { return OperatorType::PROJECT; }

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

    template<typename B>
    void Project<B>::updateCollation() {
        SortDefinition  dst_sort;

        this->getChild()->updateCollation();
        SortDefinition  src_sort_order = this->getChild()->getSortOrder();

        // *** Check to see if order-by carries over
        for(ColumnSort sort : src_sort_order) {
            int32_t src_ordinal = sort.first;
            bool found = false;
            if(src_ordinal == -1) {
                found = true; // dummy tag automatically carries over
                dst_sort.push_back(sort);
            }
            for(ProjectionMapping mapping : column_mappings_) {
                if(mapping.first == src_ordinal) {
                    dst_sort.push_back(ColumnSort (mapping.second, sort.second));
                    found = true;
                    break;
                }
            } // end search for mapping
            if(!found) {
                break;
            } // broke the sequence of mappings
        }

        this->sort_definition_ = dst_sort;
    }

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
