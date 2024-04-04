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
    using DisjointJoinPairs = std::vector<JoinPairInfo<B>>;

    template<typename B>
    class BushyPlanEnumerator {
    public:
        typedef std::tuple<SortDefinition, int /*parent_id*/, int /*child_id*/, std::string> SortEntry;// Adding operator type as a string.
        std::vector<JoinPairInfo<B>> join_pairs_;
        std::vector<JoinPair<B>> join_pairs_vector_;
        std::map<string, inputRelation<B>> sql_input_ops_;
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

        void collectSQLInputOps(Operator<B> * op);
        Operator<B>* findJoinChildFromSqlInput(string join_predicate);
        void addTransitivity();
        void addTransitivePairs(const std::pair<Operator<B> *, string> pair1, const std::pair<Operator<B> *, string> pair2, const size_t pair1_cardinality, const size_t pair2_cardinality, const std::vector<std::string> commonConditions, std::vector<JoinPairInfo<B>>& transitivePairs);
        std::vector<std::pair<int, int>> extractIntegers(const std::string& input);
        std::pair<char, size_t> getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate, const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality);
        void findJoinOfDisjointTables(const JoinPairInfo<B>& initialPair, const JoinGraph<B>& joinGraph, std::vector<JoinPairInfo<B>>& currentDisjointPairs, bool isInitialCall) ;
        std::vector<JoinPairInfo<B>> findCandidateDisjointPairs(Operator<B>* connectedOp,const std::vector<JoinPairInfo<B>>& currentDisjointPairs) const;
        bool isDisjoint(const JoinPairInfo<B>& candidatePair, const std::vector<JoinPairInfo<B>>& currentDisjointPairs) const;
        void addMissingInputs(std::vector<JoinPairInfo<B>>& currentDisjointPairs);
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

        std::string sortDirectionToString(SortDirection direction) {
            switch (direction) {
                case SortDirection::ASCENDING:
                    return "ASC";
                case SortDirection::DESCENDING:
                    return "DESC";
                case SortDirection::INVALID:
                default:
                    return "INVALID";
            }
        }

        std::string sortToString(const SortDefinition& sort) {
            // Extract the column ordinal and direction
            int32_t columnOrdinal = sort[0].first;
            SortDirection sortDirection = sort[0].second;

            // Convert both parts to string
            std::string sortInfo = std::to_string(columnOrdinal) + "-" + sortDirectionToString(sortDirection);

            return sortInfo;
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