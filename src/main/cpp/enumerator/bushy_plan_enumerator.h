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
    struct JoinPredicate {
        vector<pair<int, string>> predicates;

        // Add a new predicate if it doesn't already exist in this group
        void addPredicate(const pair<int, string>& newPredicate) {
            for (const auto& predicate : predicates) {
                if (predicate == newPredicate) {
                    return; // Already exists, so we do nothing
                }
            }
            predicates.push_back(newPredicate);
        }

        // Check if a predicate exists in this group
        bool contains(const pair<int, string>& predicate) const{
            for (const auto& p : predicates) {
                if (p == predicate) {
                    return true;
                }
            }
            return false;
        }

        // Remove a predicate from this group
        void removePredicate(const pair<int, string>& oldPredicate) {
            auto it = std::find(predicates.begin(), predicates.end(), oldPredicate);
            if (it != predicates.end()) {
                predicates.erase(it);
            }
        }
    };
    template<typename B>
    struct subPlan {
        std::shared_ptr<Operator<B>> join;
        size_t cost;
        std::string output_order_str;
        int height;

        subPlan() = default;

        subPlan(Operator<B>* join, size_t cost, std::string output_order_str, int height)
                : join(join), cost(cost), output_order_str(output_order_str), height(height) {}

    };

    template<typename B>
    struct subPlanWithKey {
        string key;
        subPlan<B> plan;

        subPlanWithKey() = default;
        subPlanWithKey(string key, subPlan<B> plan) :  key(key), plan(plan) {}
    };

    template<typename B>
    class BushyPlanEnumerator {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.
        std::vector<JoinPair<B>> join_pairs_vector_;
        std::multimap<int, inputRelation<B>> sql_input_ops_;
        std::vector<std::multimap<std::string, subPlan<B>>> memoization_table_;
        std::vector<JoinPredicate<B>> all_predicates_;
        std::vector<JoinPredicate<B>> missing_predicates_;

        BushyPlanEnumerator(Operator<B> * root, std::map<int, Operator<B> * > operators, std::vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders);

        Operator<B> *getRoot() const { return left_deep_root_; }
        Operator<B> *getOperator(const int &op_id);
        map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }
        Operator<B> *getMinCostPlan() const { return min_cost_plan_; }

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
        int next_op_id_ = 0;

        void collectSQLInputsAndJoinPairs(Operator<B> * op, std::vector<JoinPair<B>> &join_pairs);
        Operator<B>* findJoinChildFromSqlInput(string join_predicate);
        void refineJoinPairs(std::vector<JoinPair<B>>& join_pairs);
        std::string joinString(const std::vector<std::string>& strings, const std::string& delimiter);
        void addTransitivity(std::vector<JoinPair<B>>& join_pairs, JoinGraph<B>& joinGraph);
        void addTransitivePairs(Operator<B>* pair1, const string pair1_predicate, Operator<B>* pair2, const string pair2_predicate, const std::vector<std::string> commonConditions, std::vector<JoinPair<B>>& transitivePairs, JoinGraph<B>& joinGraph);
        void insertPredicate(const JoinPair<B> &joinpair);
        std::vector<std::pair<int, int>> extractIntegers(const std::string& input);
        void calculateCostsforJoinPairs(std::vector<JoinPair<B>> &joinPairs);
        std::vector<uint32_t> convertPredicateToOrdinals(const QuerySchema& schema, const string& predicates);
        int findFieldOrdinal(const QuerySchema& schema, const std::string& predicate);
        int getFKId(const std::string& lhs_predicate, const std::string& rhs_predicate);
        std::pair<char, size_t> getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate, const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality);
        void pickCheapJoinByGroup(int row_idx);
        void pickCheapJoinByGroup(std::multimap<std::string, subPlan<B>>& plan_candidates);
        void printCalculatedValues(const std::multimap<std::string, subPlan<B>> plan_candidates);
        std::vector<subPlanWithKey<B>> formatJoinPlan(const std::vector<std::tuple<int, std::string, std::string>>& path, const bool isMissingPredicates);
        Operator<B>* createBushyJoinPlan(const std::string hamiltonianPath);
        void addMissingPredicatesToJoin(std::string& join_predicate_1, std::string& join_predicate_2, const std::string& left_key, const std::string& right_key);
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

        std::vector<Operator<B>*> findEntriesWithPrefix(const int& op_id) {
            std::vector<Operator<B>*> matchingEntries;

            // Iterate over the entire container, as lower_bound usage is incorrect with just an int and previous key definition.
            for (const auto& entry : sql_input_ops_) {
                if (entry.first == op_id) {
                    matchingEntries.push_back(entry.second.table);
                }
            }
            return matchingEntries;
        }

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

        std::string addMemoizationKey(const std::string& lhsId, const std::string& rhsId) {
            // Ensure consistent ordering by comparing the complete string lexicographically
            if (lhsId.compare(rhsId) > 0)
                return "(" + rhsId + " JOIN " + lhsId + ")";
            else
                return "(" + lhsId + " JOIN " + rhsId + ")";
        }
    };
}
#endif // _BUSHY_PLAN_ENUMERATOR_H