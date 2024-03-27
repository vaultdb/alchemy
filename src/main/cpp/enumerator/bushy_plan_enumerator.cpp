#include "bushy_plan_enumerator.h"
#include <util/utilities.h>
#include <util/data_utilities.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <operators/support/aggregate_id.h>
#include "opt/operator_cost_model.h"
#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <operators/zk_sql_input.h>
#include <operators/scalar_aggregate.h>
#include <operators/keyed_join.h>
#include <operators/keyed_sort_merge_join.h>
#include <operators/merge_join.h>
#include <operators/basic_join.h>
#include <operators/filter.h>
#include <operators/project.h>
#include <parser/expression_parser.h>
#include <operators/shrinkwrap.h>
#include <operators/table_scan.h>
#include <operators/union.h>
#include <operators/left_keyed_join.h>
#include <opt/plan_optimizer.h>

#include <util/logger.h>
#include <regex>

using namespace vaultdb;
using boost::property_tree::ptree;
using namespace Logging;

template<typename B>
BushyPlanEnumerator<B>::BushyPlanEnumerator(Operator<B> * left_deep_root, map<int, Operator<B> * > operators, vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders){
    left_deep_root_ = left_deep_root;
    left_deep_root_->setParent(nullptr);
    operators_ = operators;
    support_ops_ = support_ops;
    interesting_sort_orders_ = interesting_orders;
}

template<typename B>
void BushyPlanEnumerator<B>::createBushyBalancedTree() {

    std::map<int, Operator<B> *> sqlInputOps;

    // Step 1: Collect all SQLInput operations and map them by their operator ID
    collectSQLInputOps(left_deep_root_, sqlInputOps);

    // Step 2: Recursively find join pairs starting from the left deep root
    findJoinPairs(left_deep_root_, sqlInputOps);

    // Step 3 : Add transitivity to the join_pairs_
    addTransitivity();

    // Step 4 : Sort join_pairs_ with the output cardinality of join_pairs_' 4th value(output cardainlity). Put minimum card in the first
    std::sort(join_pairs_.begin(), join_pairs_.end(), [](const JoinPairInfo<B> &a, const JoinPairInfo<B> &b) {
        return a.outputCardinality < b.outputCardinality;
    });

    // Step 5 : Create join graph from the join_pairs_
    JoinGraph<B> joinGraph = createJoinGraph(sqlInputOps, join_pairs_);

    size_t min_plan_cost_ = left_deep_root_->planCost();
    Operator<B> *min_cost_plan_ = left_deep_root_;

    // Step 6: Iterate through join pairs and create bushy plan
    for(auto it = join_pairs_.begin(); it != join_pairs_.end(); ++it) {
        JoinPairInfo<B> pair = *it;

        // Recursively find all disjoint table pairs
        findJoinOfDisjointTables(pair, joinGraph);

        // If disjoint_joins are None, then do not create bushy plan. It is not possible to create bushy plan
        if (possible_plans_.empty()) {
            continue;
        }

        // Create bushy plan
        // TODO : Implement createBushyPlan
        //auto plan = createBushyPlan(disjoint_joins, pair, join_type);

        // Calculate cost of the plan
//        auto cost = plan->planCost();
//
//        // If cost is less than min_cost, update min_cost and min_plan
//        if (cost < min_plan_cost_) {
//            min_plan_cost_ = cost;
//            min_cost_plan_ = plan;
//        }
    }
}

template<typename B>
void BushyPlanEnumerator<B>::collectSQLInputOps(Operator<B> * op, std::map<int, Operator<B> *> &sqlInputOps) {
    if (op == nullptr) return;

    if (op->getType() == OperatorType::SECURE_SQL_INPUT) {
        sqlInputOps[op->getOperatorId()] = op;
    }

    collectSQLInputOps(op->getChild(0), sqlInputOps);
    if(op->getChild(1) != nullptr)
        collectSQLInputOps(op->getChild(1), sqlInputOps);
}

template<typename B>
void BushyPlanEnumerator<B>::findJoinPairs(Operator<B> * left_deep_root, std::map<int, Operator<B> *> &sqlInputOps) {

    auto op = left_deep_root;
    while (op->getChild() != nullptr)
        op = op->getChild();

    assert(op->getType() == OperatorType::SECURE_SQL_INPUT);

    // recursively traverse this leaf to the root and create join pairs
    while (op != nullptr) {
        auto parent = op->getParent();
        if (parent != nullptr) {
            if (parent->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN ||
                parent->getType() == OperatorType::KEYED_SORT_MERGE_JOIN) {

                // find join predicates
                std::vector<std::pair<int, int>> join_predicates = extractIntegers(parent->getParameters());
                auto join_schema = parent->getOutputSchema();

                // find the join child from the SQLInput operations
                for (auto& jp : join_predicates) {
                    string lhs_predicate = join_schema.fields_[jp.first].getName();
                    string rhs_predicate = join_schema.fields_[jp.second].getName();
                    auto lhs = findJoinChildFromSqlInput(lhs_predicate, sqlInputOps);
                    auto rhs = findJoinChildFromSqlInput(rhs_predicate, sqlInputOps);

                    // Check if an existing pair matches the current LHS and RHS
                    auto it = std::find_if(join_pairs_.begin(), join_pairs_.end(), [&](const JoinPairInfo<B>& existingPair) {
                        return (existingPair.lhs.first == lhs && existingPair.rhs.first == rhs) ||
                               (existingPair.lhs.first == rhs && existingPair.rhs.first == lhs);
                    });

                    if (it != join_pairs_.end()) {
                        // An existing pair is found, combine predicates
                        it->lhs.second += " AND " + lhs_predicate; // Combine LHS predicates
                        it->rhs.second += " AND " + rhs_predicate; // Combine RHS predicates
                    } else {
                        // No existing pair, add new one
                        auto join_info = getJoinType(lhs_predicate, rhs_predicate, lhs->getOutputCardinality(), rhs->getOutputCardinality());
                        join_pairs_.push_back(JoinPairInfo<B>{std::make_pair(lhs, lhs_predicate), std::make_pair(rhs, rhs_predicate), join_info.first, join_info.second});
                    }
                }
            }
        }
        op = parent;
    }
}

template<typename B>
void BushyPlanEnumerator<B>::addTransitivity() {
    std::vector<JoinPairInfo<B>> transitivePairs;

    // Helper function to split predicates into a set of conditions
    auto splitPredicates = [](const std::string& predicateInput) -> std::set<std::string> {
        std::string predicate = predicateInput; // Make a copy of the input string to manipulate
        std::set<std::string> conditions;
        std::string delimiter = " AND ";
        size_t pos = 0;
        std::string token;

        while ((pos = predicate.find(delimiter)) != std::string::npos) {
            token = predicate.substr(0, pos); // Extract substring from beginning to delimiter
            conditions.insert(token); // Insert extracted condition
            predicate.erase(0, pos + delimiter.length()); // Erase processed part including the delimiter
        }
        // Insert the last or only condition
        if (!predicate.empty()) conditions.insert(predicate);

        return conditions;
    };


    for (auto& pair1 : join_pairs_) {
        auto conditions1 = splitPredicates(pair1.rhs.second);

        for (auto& pair2 : join_pairs_) {
            if (&pair1 == &pair2) continue;

            auto conditions2 = splitPredicates(pair2.lhs.second);

            // Find intersection of conditions to identify potential transitivity
            std::vector<std::string> commonConditions;
            std::set_intersection(conditions1.begin(), conditions1.end(),
                                  conditions2.begin(), conditions2.end(),
                                  std::back_inserter(commonConditions));

            if (!commonConditions.empty()) {
                if (pair1.lhs.first != pair2.rhs.first)
                    addTransitivePairs(pair1.lhs, pair2.rhs, pair1.outputCardinality, pair2.outputCardinality, commonConditions, transitivePairs);
                else if (pair1.lhs.first != pair2.lhs.first)
                    addTransitivePairs(pair1.lhs, pair2.lhs, pair1.outputCardinality, pair2.outputCardinality, commonConditions, transitivePairs);
                else if (pair1.rhs.first != pair2.lhs.first)
                    addTransitivePairs(pair1.rhs, pair2.lhs, pair1.outputCardinality, pair2.outputCardinality, commonConditions, transitivePairs);
                else if (pair1.rhs.first != pair2.rhs.first)
                    addTransitivePairs(pair1.rhs, pair2.rhs, pair1.outputCardinality, pair2.outputCardinality, commonConditions, transitivePairs);
            }
        }
    }

    // Incorporate transitive join pairs into the existing list of join pairs
    join_pairs_.insert(join_pairs_.end(), transitivePairs.begin(), transitivePairs.end());
}

template<typename B>
void BushyPlanEnumerator<B>::addTransitivePairs(const std::pair<Operator<B> *, string> pair1, const std::pair<Operator<B> *, string> pair2, const size_t pair1_cardinality, const size_t pair2_cardinality, const std::vector<std::string> commonConditions, std::vector<JoinPairInfo<B>>& transitivePairs){
    // Identified a transitive relationship based on common predicates
    auto joinInfo = getJoinType(pair1.second, pair2.second, pair1_cardinality, pair2_cardinality);

    // Helper function to split predicates into a set of conditions
    auto splitPredicates = [](const std::string& predicateInput) -> std::set<std::string> {
        std::string predicate = predicateInput; // Make a copy of the input string to manipulate
        std::set<std::string> conditions;
        std::string delimiter = " AND ";
        size_t pos = 0;
        std::string token;

        while ((pos = predicate.find(delimiter)) != std::string::npos) {
            token = predicate.substr(0, pos); // Extract substring from beginning to delimiter
            conditions.insert(token); // Insert extracted condition
            predicate.erase(0, pos + delimiter.length()); // Erase processed part including the delimiter
        }
        // Insert the last or only condition
        if (!predicate.empty()) conditions.insert(predicate);

        return conditions;
    };

    std::set<std::string> pair1_predicate = splitPredicates(pair1.second);
    std::set<std::string> pair2_predicate = splitPredicates(pair2.second);

    for(auto it = commonConditions.begin(); it != commonConditions.end(); ++it){
        // compare commonConditions with pair1_predicate and pair2_predicate after '_' for both string,
        // if not same, then remove the condition from that pair_predicate
        for (auto jp = pair1_predicate.begin(); jp != pair1_predicate.end();) {
            if (it->substr(it->find('_') + 1) != jp->substr(jp->find('_') + 1)) {
                // Erase and update the iterator to the next valid element
                jp = pair1_predicate.erase(jp);
            } else {
                // Only increment the iterator if no erasure took place
                ++jp;
            }
        }

        // Repeat for pair2_predicate with the same logic
        for (auto jp = pair2_predicate.begin(); jp != pair2_predicate.end();) {
            if (it->substr(it->find('_') + 1) != jp->substr(jp->find('_') + 1)) {
                jp = pair2_predicate.erase(jp);
            } else {
                ++jp;
            }
        }
    }

    // Construct a new JoinPairInfo with lhs from pair1 and rhs from pair2
    JoinPairInfo<B> newTransitivePair{
            {pair1.first,
             joinPredicates(pair1_predicate)}, // Example condition combination
            {pair2.first,
             joinPredicates(pair2_predicate)}, // Needs refinement
            joinInfo.first,
            joinInfo.second
    };

    // Add to the list of transitive pairs
    transitivePairs.push_back(newTransitivePair);
}


template<typename B>
Operator<B> *BushyPlanEnumerator<B>::findJoinChildFromSqlInput(string join_predicate, std::map<int, Operator<B> *> &sqlInputOps) {
    for(auto it = sqlInputOps.begin(); it != sqlInputOps.end(); ++it) {
        Operator<B> *sqlInputOp = it->second;
        auto sqlInputSchema = sqlInputOp->getOutputSchema();
        if(sqlInputSchema.hasField(join_predicate))
            return sqlInputOp;
    }
}

template<typename B>
std::pair<char, size_t> BushyPlanEnumerator<B>::getJoinType(const std::string& lhs_predicate, const std::string& rhs_predicate,
                                                            const size_t& lhs_output_cardinality, const size_t& rhs_output_cardinality) {
    std::vector<std::string> PKList = {"n_nationkey", "r_regionkey", "s_suppkey", "c_custkey", "p_partkey", "ps_partkey", "ps_suppkey", "o_orderkey", "l_orderkey", "l_linenumber"};
    bool lhs_found = std::find(PKList.begin(), PKList.end(), lhs_predicate) != PKList.end();
    bool rhs_found = std::find(PKList.begin(), PKList.end(), rhs_predicate) != PKList.end();

    if (lhs_found) {
        // Return rhs's output cardinality and 'K'
        return std::make_pair('K', rhs_output_cardinality);
    } else if (rhs_found) {
        // Return lhs's output cardinality and 'K'
        return std::make_pair('K', lhs_output_cardinality);
    } else {
        // Return lhs's output cardinality * rhs's output cardinality and 'N'
        return std::make_pair('N', lhs_output_cardinality * rhs_output_cardinality);
    }
}

template<typename B>
std::vector<std::pair<int, int>> BushyPlanEnumerator<B>::extractIntegers(const std::string& input) {
    std::vector<std::pair<int, int>> integerPairs;

    // Adjusted regex pattern to closely match the successful Python version
    std::regex regex("P\\$(\\d+)\\(\\d+\\)\\s*==\\s*P\\$(\\d+)\\(\\d+\\)");

    // Using std::sregex_iterator to iterate through all matches
    std::sregex_iterator begin(input.begin(), input.end(), regex);
    std::sregex_iterator end;

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        int firstInt = std::stoi(match[1].str());
        int secondInt = std::stoi(match[2].str());
        integerPairs.push_back(std::make_pair(firstInt, secondInt));
    }

    return integerPairs;
}

template<typename B>
void BushyPlanEnumerator<B>::findJoinOfDisjointTables(const JoinPairInfo<B>& initialPair, const JoinGraph<B>& joinGraph) {
    std::vector<JoinPairInfo<B>> initialPairs{initialPair};
    auto connectedOperatorsWithLinkages = joinGraph.getConnectedOperators(initialPairs, join_pairs_);

    for (auto& [op, linkagePairs] : connectedOperatorsWithLinkages) {
        std::vector<JoinPairInfo<B>> foundDisjointPairs;
        if (findDisjointPair(op, linkagePairs, foundDisjointPairs)) {
            // For each found disjoint pair, create a SubJoinPlan or similar structure
            for (auto& newPair : foundDisjointPairs) {
                // Example: Building a vector of SubJoinPlans or similar structure
                // This will likely need to be aggregated into a larger structure like possible_plans_ or equivalent
            }
        }
    }
    // Further actions or recursive calls as needed
}

//
//template<typename B>
//void BushyPlanEnumerator<B>::recursivelyFindDisjointPairs(std::vector<JoinPairInfo<B>>& derivedJoinPairs, const JoinGraph<B>& joinGraph) {
//    bool foundNewPair = false;
//    for (const auto& pair : derivedJoinPairs) {
//        std::vector<Operator<B>*> connectedOperators = joinGraph.getConnectedOperators(pair);
//        for (Operator<B>* op : connectedOperators) {
//            std::vector<JoinPairInfo<B>> newDisjointPairs;
//            if (findDisjointPair(op, pair, derivedJoinPairs, newDisjointPairs)) {
//                foundNewPair = true;
//                for (const auto& newPair : newDisjointPairs) {
//                    SubJoinPlan<B> subPlan{pair, newPair, {/* linkage info here based on op */}};
//                    possible_plans_.push_back({subPlan, /* rightJoinPair if applicable */, {/* linkages if applicable */}});
//                    // Consider recursive call with newDisjointPairs to explore deeper connections
//                }
//            }
//        }
//    }
//
//    if (!foundNewPair) {
//        // Finalize and store the current plan if no new pairs can be connected
//        //finalizeCurrentPlan(derivedJoinPairs);
//    }
//}

template<typename B>
bool BushyPlanEnumerator<B>::findDisjointPair(
        Operator<B>* op,
        const std::vector<JoinPairInfo<B>>& linkagePairs,
        std::vector<JoinPairInfo<B>>& resultPairs) {
    bool found = false;

    for (const auto& linkagePair : linkagePairs) { // Iterate through each linkage pair
        for (const auto& candidatePair : join_pairs_) {
            if ((candidatePair.lhs.first == op || candidatePair.rhs.first == op) &&
                isDisjointFromLinkagePair(candidatePair, linkagePair)) {
                resultPairs.push_back(candidatePair);
                found = true;
            }
        }
    }
    return found;
}


template<typename B>
bool BushyPlanEnumerator<B>::isDisjointFromLinkagePair(
        const JoinPairInfo<B>& candidatePair,
        const JoinPairInfo<B>& linkagePair) {
    // A candidate pair is disjoint from a linkage pair if it does not overlap with it
    // except possibly at the connection point (operator) between them.

    // Check if the candidate pair's lhs or rhs is the same as the linkage pair's lhs or rhs,
    // which would mean they are directly connected or possibly overlapping.
    bool lhsOverlap = candidatePair.lhs.first == linkagePair.lhs.first || candidatePair.lhs.first == linkagePair.rhs.first;
    bool rhsOverlap = candidatePair.rhs.first == linkagePair.lhs.first || candidatePair.rhs.first == linkagePair.rhs.first;

    // If both lhs and rhs of the candidate pair overlap with the linkage pair, it's not disjoint.
    if (lhsOverlap && rhsOverlap) {
        return false; // Not disjoint because it fully overlaps with the linkage pair.
    }

    // Additional checks can be implemented here if there are more complex conditions
    // for determining disjointness in your specific application or data model.

    return true; // Considered disjoint if it doesn't fully overlap with the linkage pair.
}






template<typename B>
JoinGraph<B> BushyPlanEnumerator<B>::createJoinGraph(const std::map<int, Operator<B> *> &sqlInputOps, const std::vector<JoinPairInfo<B>>& joinPairs) {
    JoinGraph<B> joinGraph;

    // Step 1: Add all operators as nodes
    for (const auto& opPair : sqlInputOps) {
        joinGraph.addNode(opPair.second);
    }

    // Step 2: Add edges based on join pairs
    for (const auto& joinPair : joinPairs) {
        // Directly use Operator<B>* pointers to add edges since all operators are already added as nodes
        joinGraph.addEdge(joinPair.lhs.first, joinPair.rhs.first, joinPair.joinType);
    }

    return joinGraph;
}



template class vaultdb::BushyPlanEnumerator<bool>;
template class vaultdb::BushyPlanEnumerator<emp::Bit>;