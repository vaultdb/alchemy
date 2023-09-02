#ifndef _PLAN_DEPARSER_
#define _PLAN_DEPARSER_

#include "operators/operator.h"
#include "operators/join.h"
#include <boost/property_tree/ptree.hpp>
#include "expression/expression.h"
#include "expression_deparser.h"
#include "operators/group_by_aggregate.h"
#include "operators/merge_join.h"

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::basic_ptree;

namespace pt = boost::property_tree;

namespace vaultdb {
// converts a query tree into a JSON plan
    template<typename B>
    class PlanDeparser {
    public:
        PlanDeparser(Operator<B> *root): root_(root) {
            validateTree();
            deparseTree();
        }

        string getJsonPlan() const { return json_plan_; }

        static string deparse(Operator<B> *root) {
            PlanDeparser<B> deparser(root);
            return deparser.getJsonPlan();
        }


    private:
        string json_plan_;
        Operator<B> *root_;
        pt::ptree rels_;

        void deparseTree();
        void deparseTreeHelper(const Operator<B> *node);

        pt::ptree deparseNode(const Operator<B> *node);
        pt::ptree deparseCollation(const SortDefinition & sort);

        pt::ptree deparseSecureSqlInput(const Operator<B> *input);
        pt::ptree deparseSort(const Operator<B> *sort);
        pt::ptree deparseNestedLoopJoin(const Operator<B> *join);
        pt::ptree deparseFilter(const Operator<B> *filter);
        pt::ptree deparseProject(const Operator<B> *project);
        pt::ptree deparseTableInput(const Operator<B> *input);
        pt::ptree deparseCsvInput(const Operator<B> *input);
        pt::ptree deparseUnion(const Operator<B> *input);
        pt::ptree deparseShrinkwrap(const Operator<B> *input);
        pt::ptree deparseSortMergeAggregate(const Operator<B> *input);
        pt::ptree deparseNestedLoopAggregate(const Operator<B> *input);
        pt::ptree deparseSortMergeJoin(const Operator<B> *input);
        pt::ptree deparseMergeJoin(const Operator<B> *input);
        pt::ptree deparseKeyedNestedLoopJoin(const Operator<B> *input);
        pt::ptree deparseSqlInput(const Operator<B> *input);
        pt::ptree deparseZkSqlInput(const Operator<B> *input);
        pt::ptree deparseScalarAggregate(const Operator<B> *agg);
        pt::ptree deparseMergeInput(const Operator<B> *input);

        void validateTree();
        void validateTreeHelper(Operator<B> *node, map<int, int> & known_ids);

        // creates a list of field1Name (#0), field2Name (#1), etc.
       inline string generateFieldList(const QuerySchema & schema) {
            int field_cnt = schema.getFieldCount();
            if (field_cnt == 0) return "";
            string fields = schema.getField(0).getName() + " (#0)";

            for (int i = 1; i < field_cnt; ++i) {
                fields += ", " + schema.getField(i).getName() + " (#" + to_string(i) + ")";
            }

            return fields;
        }

        pt::ptree deparseSchema(const QuerySchema & schema);

        ptree deparseJoin(const Join<B> *join, const string &algo_name, const int &foreign_key_reln) {
            pt::ptree join_node;
            writeHeader(join_node, join, "LogicalJoin");
            join_node.put("operator-algorithm", algo_name);
            if(foreign_key_reln == 0 || foreign_key_reln == 1) {
                join_node.put("foreign-key", foreign_key_reln);
            }

            ptree join_cond = ExpressionDeparser<B>::deparse(join->getPredicate());
            join_node.add_child("condition", join_cond);
            // TODO: add OR handling for merge join

            if(join->getType() == OperatorType::MERGE_JOIN) {
                //  "dummy-handling" : "OR",
                auto mj = (MergeJoin<B> *) join;
                join_node.put("dummy-handling", mj->getDummyHandling() ? "OR" : "AND");
            }

            return join_node;
       }


        void writeHeader(ptree & to_write, const Operator<B> *op, string op_name) {
            to_write.put("id", op->getOperatorId());
            to_write.put("relOp", op_name);
            to_write.put("outputFields", generateFieldList(op->getOutputSchema()));

            ptree inputs;
            if(op->getChild() != nullptr) inputs.push_back(std::make_pair("", ptree(std::to_string(op->getChild()->getOperatorId()))));
            if(op->getChild(1) != nullptr) inputs.push_back(std::make_pair("", ptree(std::to_string(op->getChild(1)->getOperatorId()))));
            if(op->getChild() != nullptr) {
                to_write.add_child("inputs", inputs);
            }
       }

       ptree deparseGroupByAggregate(GroupByAggregate<B> *agg, const string & algo_name) {
           pt::ptree gb_agg_node;
           writeHeader(gb_agg_node, agg, "LogicalAggregate");
           gb_agg_node.put("operator-algorithm", algo_name);
           gb_agg_node.put<int>("cardinality-bound", agg->getCardinalityBound());

           pt::ptree group_bys;
           for(auto col : agg->group_by_) {
               pt::ptree col_ordinal;
               col_ordinal.put_value(col);
               group_bys.push_back(std::make_pair("", col_ordinal));
           }

           gb_agg_node.add_child("group", group_bys);

            pt::ptree aggregates;
            for(ScalarAggregateDefinition agg_def : agg->aggregate_definitions_) {
                ptree agg_node = ScalarAggregateDefinition::aggregateDefinitionToPTree(agg_def, agg->getOutputSchema());
                aggregates.push_back(std::make_pair("", agg_node));
            }
            gb_agg_node.add_child("aggs", aggregates);

            auto collation = deparseCollation(agg->effective_sort_);
            gb_agg_node.add_child("effective-collation", collation);

           return gb_agg_node;

       }





    };



}
#endif
