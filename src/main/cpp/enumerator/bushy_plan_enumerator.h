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
#include <enumerator/join_graph.h>

namespace vaultdb {
    template<typename B>
    struct SubJoinPlan {
        JoinPairInfo<B> leftJoinPair;
        JoinPairInfo<B> rightJoinPair;  // The join pairs involved in this plan.
        std::vector<JoinPairInfo<B>> linkages; // Direct linkages between join pairs.

        SubJoinPlan(JoinPairInfo<B> leftJoinPair, JoinPairInfo<B> rightJoinPair, std::vector<JoinPairInfo<B>> linkages)
                : leftJoinPair(leftJoinPair), rightJoinPair(rightJoinPair), linkages(linkages) {}

        // Adds a direct linkage between two join pairs.
        void addLinkage(const JoinPairInfo<B>& joinPair) {
            linkages.push_back(joinPair);
        }
    };

    template<typename B>
    struct JoinPlan {
        SubJoinPlan<B> leftSubJoinPlan;
        JoinPairInfo<B> rightJoinPair; // The join pairs involved in this plan.
        std::vector<JoinPairInfo<B>> linkages; // Direct linkages between join pairs.

        JoinPlan(SubJoinPlan<B> leftSubJoinPlan, JoinPairInfo<B> rightJoinPair, std::vector<JoinPairInfo<B>> linkages)
                : leftSubJoinPlan(leftSubJoinPlan), rightJoinPair(rightJoinPair), linkages(linkages) {}

        // Adds a direct linkage between two join pairs.
        void addLinkage(const JoinPairInfo<B>& joinPair) {
            linkages.push_back(joinPair);
        }
    };

    template<typename B>
    class BushyPlanEnumerator {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.
        std::vector<JoinPairInfo<B>> join_pairs_;

        BushyPlanEnumerator(Operator<B> * root, std::map<int, Operator<B> * > operators, std::vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders);

        Operator<B> *getRoot() const { return left_deep_root_; }
        Operator<B> *getOperator(const int &op_id);
        map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }

        void createBushyBalancedTree();
        std::vector<JoinPlan<B>> possible_plans_;

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
        void addTransitivity();
        void addTransitivePairs(const std::pair<Operator<B> *, string> pair1, const std::pair<Operator<B> *, string> pair2, const size_t pair1_cardinality, const size_t pair2_cardinality, const std::vector<std::string> commonConditions, std::vector<JoinPairInfo<B>>& transitivePairs);
        Operator<B>* findJoinChildFromSqlInput(string join_predicate, std::map<int, Operator<B> *> &sqlInputOps);
        std::vector<std::pair<int, int>> extractIntegers(const std::string& input);
        std::pair<char, size_t> getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate, const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality);
        void findJoinOfDisjointTables(const JoinPairInfo<B>& initialPair, const JoinGraph<B>& joinGraph) ;
        bool findDisjointPair(Operator<B>* op, const std::vector<JoinPairInfo<B>>& linkagePairs, std::vector<JoinPairInfo<B>>& resultPairs);
        bool isDisjointFromLinkagePair(const JoinPairInfo<B>& candidatePair, const JoinPairInfo<B>& linkagePair);
        JoinGraph<B> createJoinGraph(const std::map<int, Operator<B> *> &sqlInputOps, const std::vector<JoinPairInfo<B>>& joinPairs);
        std::string joinPredicates(const std::set<std::string>& predicates) {
            std::string result;
            for (auto it = predicates.begin(); it != predicates.end(); ++it) {
                if (it != predicates.begin()) {
                    result += " AND ";
                }
                result += *it;
            }
            return result;
        }
    };
}
#endif // _BUSHY_PLAN_ENUMERATOR_H