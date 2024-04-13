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
    struct SqlInputKey{
        int op_id;
        SortDefinition sort_orders;

        SqlInputKey(int op_id, SortDefinition sort_orders) : op_id(op_id), sort_orders(sort_orders) {}

        // Comparison operator
        bool operator<(const SqlInputKey& other) const {
            if (op_id != other.op_id) {
                return op_id < other.op_id;
            }

            // Now, compare SortDefinition
            const auto& lhs = this->sort_orders;
            const auto& rhs = other.sort_orders;

            for (size_t i = 0; i < std::min(lhs.size(), rhs.size()); ++i) {
                // Compare ordinal
                if (lhs[i].first != rhs[i].first) {
                    return lhs[i].first < rhs[i].first;
                }
                // If ordinals are equal, compare direction
                if (lhs[i].second != rhs[i].second) {
                    return lhs[i].second < rhs[i].second;
                }
            }

            // If all compared elements are equal, the shorter SortDefinition is considered "less than" if they are of different lengths
            return lhs.size() < rhs.size();
        }
    };

    template<typename B>
    struct MemoIndex {
        // Option 1: Direct reference to sql_input_ops_ entry (e.g., by SqlInputKey or UID)
        // Option 2: Pair of integers to reference memoization_table_ entries (row, column)
        std::variant<SqlInputKey<B>, std::pair<int, int>> index;

        // Constructor for SqlInputKey<B>
        MemoIndex(const SqlInputKey<B>& key) : index(key) {}

        // Constructor for a pair of integers
        MemoIndex(int row, int col) : index(std::make_pair(row, col)) {}
    };

    template<typename B>
    struct subPlan {
        MemoIndex<B> lhs;
        MemoIndex<B> rhs;
        OperatorType type;
        size_t cost;
        SortDefinition output_order;
        std::string output_order_str;

        subPlan(MemoIndex<B> lhs, MemoIndex<B> rhs, OperatorType type, size_t cost, SortDefinition output_order, std::string output_order_str)
                : lhs(lhs), rhs(rhs), type(type), cost(cost), output_order(output_order), output_order_str(output_order_str) {}
    };

    // Definition for the overloaded helper
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template<typename B>
    using DisjointJoinPairs = std::vector<JoinPairInfo<B>>;

    template<typename B>
    class BushyPlanEnumerator {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.
        std::vector<JoinPairInfo<B>> join_pairs_;
        std::vector<JoinPair<B>> join_pairs_vector_;
        std::map<SqlInputKey<B>, inputRelation<B>> sql_input_ops_;
        std::vector<std::vector<subPlan<B>>> memoization_table_;

        BushyPlanEnumerator(Operator<B> * root, std::map<int, Operator<B> * > operators, std::vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders);

        Operator<B> *getRoot() const { return left_deep_root_; }
        Operator<B> *getOperator(const int &op_id);
        map<int, vector<SortDefinition>> getInterestingSortOrders() { return interesting_sort_orders_; }

        void createBushyBalancedTree();
        std::vector<DisjointJoinPairs<B>> possible_disjoint_pairs_;

        int total_plan_cnt_ = 0;
    private:

        Operator<B> *left_deep_root_;

        // plan enumerator state
        map<int, vector<SortDefinition>> interesting_sort_orders_;
        size_t min_plan_cost_ = std::numeric_limits<size_t>::max();
        Operator<B> *min_cost_plan_ = nullptr;
        std::map<int, Operator<B> * > operators_; // op ID --> operator instantiation
        std::vector<Operator<B> * > support_ops_; // these ones don't get an operator ID from the JSON plan

        void collectSQLInputsAndJoinPairs(Operator<B> * op, std::vector<JoinPair<B>> &join_pairs);
        Operator<B>* findJoinChildFromSqlInput(string join_predicate);
        void addTransitivity(std::vector<JoinPair<B>>& join_pairs, JoinGraph<B>& joinGraph);
        void addTransitivePairs(Operator<B>* pair1, const string pair1_predicate, Operator<B>* pair2, const string pair2_predicate, const std::vector<std::string> commonConditions, std::vector<JoinPair<B>>& transitivePairs, JoinGraph<B>& joinGraph);
        std::vector<std::pair<int, int>> extractIntegers(const std::string& input);
        void calculateCostsforJoinPairs(std::vector<JoinPair<B>> &joinPairs);
        std::vector<uint32_t> convertPredicateToOrdinals(const QuerySchema& schema, const string& predicates);
        boost::property_tree::ptree createJoinConditionTree(const QuerySchema& lhs, const QuerySchema& rhs, const string& lhs_predicate, const string& rhs_predicate);
        boost::property_tree::ptree createSimpleConditionTree(const QuerySchema& input_schema, const std::string& lhs_predicate, const std::string& rhs_predicate);
        int findFieldOrdinal(const QuerySchema& schema, const std::string& predicate);
        int getFKId(const std::string& lhs_predicate, const std::string& rhs_predicate);
        std::pair<char, size_t> getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate, const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality);
        void pickCheapJoinByGroup(int row_idx);
        Operator<B>* createBushyJoinPlan(const std::string hamiltonianPath);
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

        std::vector<std::pair<SqlInputKey<B>, inputRelation<B>>> findEntriesWithPrefix(const int& op_id) {
            std::vector<std::pair<SqlInputKey<B>, inputRelation<B>>> matchingEntries;

            // Iterate over the entire container, as lower_bound usage is incorrect with just an int and previous key definition.
            for (const auto& entry : sql_input_ops_) {
                if (entry.first.op_id == op_id) {
                    matchingEntries.push_back(entry);
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
    };
}
#endif // _BUSHY_PLAN_ENUMERATOR_H