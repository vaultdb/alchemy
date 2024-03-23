#ifndef _BUSHY_PLAN_ENUMERATOR_H
#define _BUSHY_PLAN_ENUMERATOR_H

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
    struct JoinPairInfo {
        std::pair<Operator<B> *, string> lhs;
        std::pair<Operator<B> *, string> rhs;
        char joinType;
        int outputCardinality;

        JoinPairInfo(std::pair<Operator<B> *, string> lhs, std::pair<Operator<B> *, string> rhs, char joinType, int outputCardinality)
                : lhs(lhs), rhs(rhs), joinType(joinType), outputCardinality(outputCardinality) {}
    };

    template<typename B>
    class BushyPlanEnumerator {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.
        std::vector<JoinPairInfo<B>> join_pairs;

        BushyPlanEnumerator(Operator<B> * root, std::map<int, Operator<B> * > operators, std::vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders);

        Operator<B> *getRoot() const { return left_deep_root_; }
        Operator<B> *getOperator(const int &op_id);
        map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }

        void createBushyBalancedTree();

        int total_plan_cnt_ = 0;
    private:

        Operator<B> *left_deep_root_;

        // plan enumerator state
        map<int, vector<SortDefinition>> interesting_sort_orders_;
        size_t min_plan_cost_ = std::numeric_limits<size_t>::max();
        Operator<B> *min_cost_plan_ = nullptr;
        std::map<int, Operator<B> * > operators_; // op ID --> operator instantiation
        std::vector<Operator<B> * > support_ops_; // these ones don't get an operator ID from the JSON plan

        void collectSQLInputOps(Operator<B> * op, std::map<int, Operator<B> *> &sqlInputOps);
        void findJoinPairs(Operator<B> * left_deep_root, std::map<int, Operator<B> *> &sqlInputOps);
        Operator<B>* findJoinChildFromSqlInput(string join_predicate, std::map<int, Operator<B> *> &sqlInputOps);
        std::vector<std::pair<int, int>> extractIntegers(const std::string& input);
        std::pair<char, size_t> getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate, const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality);
        std::vector<JoinPairInfo<B>> findJoinOfDisjointTables(JoinPairInfo<B> selectedPair);
    };




}


#endif // _BUSHY_PLAN_ENUMERATOR_H