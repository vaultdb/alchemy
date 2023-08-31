#ifndef _PLAN_DEPARSER_
#define _PLAN_DEPARSER_

#include "operators/operator.h"
#include <boost/property_tree/ptree.hpp>

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::basic_ptree;

namespace pt = boost::property_tree;

namespace vaultdb {
// converts a query tree into a JSON plan
    template<typename B>
    class PlanDeparser {
    public:
        PlanDeparser(const Operator<B> *root): root_(root) {
            deparseTree();
        }

        string getJsonPlan() const { return json_plan_; }

        static string deparse(const Operator<B> *root) {
            PlanDeparser<B> deparser(root);
            return deparser.getJsonPlan();
        }


    private:
        string json_plan_;
        const Operator<B> *root_;
        pt::ptree rels_;

        void deparseTree();
        void deparseTreeHelper(const Operator<B> *node);

        pt::ptree deparseNode(const Operator<B> *node);
        pt::ptree deparseCollation(const SortDefinition & sort);
        pt::ptree deparseSchema(const QuerySchema & schema);

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

        void eraseValueQuotes(string &line, const vector<string> & search_strs) const {
            for(auto search_str : search_strs) {
                if(line.find(search_str) != std::string::npos) {
                    // delete last two instances of quote
                    line.erase(line.find_last_of("\""), 1);
                    line.erase(line.find_last_of("\""), 1);

                }
            }
        }


    };



}
#endif
