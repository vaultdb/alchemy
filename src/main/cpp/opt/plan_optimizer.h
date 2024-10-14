#ifndef _PLAN_OPTIMIZER_H
#define _PLAN_OPTIMIZER_H

#include <string>
#include <tuple>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <emp-tool/emp-tool.h>
#include <operators/operator.h>
#include "util/system_configuration.h"
#include <operators/sort.h>
#include <operators/merge_input.h>

namespace vaultdb {

    template<typename B> class SortMergeAggregate;
    template<typename B> class NestedLoopAggregate;

    template<typename B>
    class PlanOptimizer {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.

        PlanOptimizer(Operator<B> * root, std::map<int, Operator<B> * > operators, std::vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders);

        Operator<B> *getRoot() const { return root_; }

        Operator<B> *getOperator(const int &op_id);

        Operator<B> *optimizeTree();

        map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }
        int total_plan_cnt_ = 0;
    private:

        Operator<B> *root_;

        // plan enumerator state
        map<int, vector<SortDefinition>> interesting_sort_orders_;
        size_t min_plan_cost_ = std::numeric_limits<size_t>::max();
        Operator<B> *min_cost_plan_ = nullptr;
        std::map<int, Operator<B> * > operators_; // op ID --> operator instantiation
        std::vector<Operator<B> * > support_ops_; // these ones don't get an operator ID from the JSON plan

        void calculateAutoAggregate();


        void optimizeTreeHelper(Operator<B> *child);

        void recurseNode(Operator<B> *op);

        Operator<B> *fetchLeaf(Operator<B> *op);

        void recurseJoin(Operator<B> *join);
        void enumerateJoin(Operator<B>* join, Operator<B>* lhs_clone, Operator<B>* rhs_clone);

        void recurseAgg(Operator<B> *agg);
        void recurseSort(Operator<B> *sort);

        vector<SortDefinition> getCollations(Operator<B> *op) {
            map<SortDefinition, int> collations; // making a map to eliminate duplicate collations
            collations[SortDefinition()] = 0; // empty set
            collations[op->getSortOrder()] = 0; // default sort
            for (auto collation: interesting_sort_orders_[op->getOperatorId()]) {
                collations[collation] = 0;
            }

            vector<SortDefinition> sorts;
            for (auto collation: collations) {
                sorts.push_back(collation.first);
            }
            return sorts;
        }


    };



}


#endif // _PLAN_OPTIMIZER_H