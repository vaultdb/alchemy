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
    memoization_table_.resize(2);
    next_op_id_ = operators.size();
}

template<typename B>
void BushyPlanEnumerator<B>::createBushyBalancedTree() {

    // Step 1: Collect all SQLInput operations and map them by their operator ID
    // find join pairs from the left deep tree, and store them in join_pairs_vector
    std::vector<JoinPair<B>> join_pairs;
    collectSQLInputsAndJoinPairs(left_deep_root_, join_pairs);
    refineJoinPairs(join_pairs);

    // Step 2 : Add transitivity to the join_pairs_ and create join graph
    JoinGraph<B> joinGraph;
    addTransitivity(join_pairs, joinGraph);

    // Step 3 : Calcuate join cost for each join pair
    calculateCostsforJoinPairs(join_pairs);
    printCalculatedValues(memoization_table_[0]);

    // Step 4 : group join pairs by their output sort order in memoization_table_[0]
    pickCheapJoinByGroup(0);

    std::cout << "After picking cheap join by group" << std::endl;
    printCalculatedValues(memoization_table_[0]);

    // Step 5 : Find all paths from join graph.
    std::vector<string> hamiltonian_paths = joinGraph.findHamiltonianPaths();

    // Step 6 : Iterate hamiltonian path and create bushy plan
    // Prune the paths that are the same bushy plan (i.e., rhs lhs swapped)

    // If there are no hamiltonian paths, then bushy plan is not possible
    if(hamiltonian_paths.size() == 0)
        std::cout << "Bushy plan is not possible" << std::endl;
    else {
        for (auto path: hamiltonian_paths) {
            Operator<B> *plan = createBushyJoinPlan(path);
            size_t cost = plan->planCost();
            if (cost < min_plan_cost_) {
                min_plan_cost_ = cost;
                min_cost_plan_ = plan;
            }
        }
    }
    std::cout << "Bushy plan enumeration completed" << std::endl;
}

template<typename B>
void BushyPlanEnumerator<B>::collectSQLInputsAndJoinPairs(Operator<B> * op, std::vector<JoinPair<B>> &join_pairs) {
    if (op == nullptr) return;

    if (op->getType() == OperatorType::SECURE_SQL_INPUT) {
        int op_id = op->getOperatorId();
        auto sorts = getCollations(op);
        for (auto &sort: sorts) {
            Operator<B>* node = op->clone();
            node->setSortOrder(sort);

            // Add the SQLInput operator to the map
            sql_input_ops_.insert(std::make_pair(op_id, inputRelation<B>(node, node->planCost())));
        }
    }
    else if(op->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN || op->getType() == OperatorType::KEYED_SORT_MERGE_JOIN){
        // find join predicates
        std::vector<std::pair<int, int>> join_predicates = extractIntegers(op->getParameters());
        auto join_schema = op->getOutputSchema();

        Operator<B> *lhs = op->getChild(0);
        Operator<B> *rhs = op->getChild(1);

        std::string lhs_predicates = join_schema.fields_[join_predicates[0].first].getName();
        std::string rhs_predicates = join_schema.fields_[join_predicates[0].second].getName();

        // Determine if the operator types are not SECURE_SQL_INPUT and update lhs and rhs accordingly
        if (lhs->getType() != OperatorType::SECURE_SQL_INPUT)
            lhs = findJoinChildFromSqlInput(lhs_predicates);

        if (rhs->getType() != OperatorType::SECURE_SQL_INPUT)
            rhs = findJoinChildFromSqlInput(rhs_predicates);

        for (int idx = 1; idx < join_predicates.size(); idx++) {
            lhs_predicates += " AND " + join_schema.fields_[join_predicates[idx].first].getName();
            rhs_predicates += " AND " + join_schema.fields_[join_predicates[idx].second].getName();
        }

        // Finally, add the join pair with the concatenated predicates
        join_pairs.push_back(JoinPair<B>{lhs, rhs, lhs_predicates, rhs_predicates});
    }

    collectSQLInputsAndJoinPairs(op->getChild(0), join_pairs);
    if(op->getChild(1) != nullptr)
        collectSQLInputsAndJoinPairs(op->getChild(1), join_pairs);
}

template<typename B>
Operator<B> *BushyPlanEnumerator<B>::findJoinChildFromSqlInput(string join_predicate) {
    for(auto it = operators_.begin(); it != operators_.end(); ++it){
        Operator<B> *op = it->second;
        if(op->getType() == OperatorType::SECURE_SQL_INPUT){
            auto sqlInputSchema = op->getOutputSchema();
            if(sqlInputSchema.hasField(join_predicate))
                return op;
        }
    }
    return nullptr;
}

template<typename B>
void BushyPlanEnumerator<B>::refineJoinPairs(std::vector<JoinPair<B>>& join_pairs) {
    std::vector<JoinPair<B>> refined_pairs;
    std::vector<JoinPair<B>> new_pairs;

    auto splitPredicates = [](const std::string& predicates) -> std::vector<std::string> {
        std::vector<std::string> result;
        size_t pos = 0;
        std::string delimiter = " AND ";
        std::string copyPredicates = predicates; // Create a copy to avoid modifying the original string

        while ((pos = copyPredicates.find(delimiter)) != std::string::npos) {
            std::string token = copyPredicates.substr(0, pos);
            // Remove leading and trailing spaces
            token.erase(0, token.find_first_not_of(" "));
            token.erase(token.find_last_not_of(" ") + 1);
            result.push_back(token);
            copyPredicates.erase(0, pos + delimiter.length());
        }
        // Add the last element after parsing
        std::string lastToken = copyPredicates;
        lastToken.erase(0, lastToken.find_first_not_of(" "));
        lastToken.erase(lastToken.find_last_not_of(" ") + 1);
        if (!lastToken.empty()) {
            result.push_back(lastToken);
        }
        return result;
    };

    for (auto& pair : join_pairs) {
        std::vector<std::string> lhs_predicates = splitPredicates(pair.lhs_predicate);
        std::vector<std::string> rhs_predicates = splitPredicates(pair.rhs_predicate);

        assert(lhs_predicates.size() == rhs_predicates.size());
        std::vector<std::pair<std::string, std::string>> predicatePairs;
        for(int idx = 0; idx < lhs_predicates.size(); idx++)
            predicatePairs.emplace_back(lhs_predicates[idx], rhs_predicates[idx]);


        // Filter predicates based on schema
        auto filterValidPredicates = [&](const std::vector<std::pair<std::string, std::string>>& predicatePairs, Operator<B>* lhs_op, Operator<B>* rhs_op) {
            std::vector<std::pair<std::string, std::string>> validPairs;
            std::vector<std::pair<std::string, std::string>> invalidPairs;

            // Check pairs
            for (const auto& pair : predicatePairs) {
                const auto& lhs_predicate = pair.first;
                const auto& rhs_predicate = pair.second;

                bool lhs_valid = lhs_op->getOutputSchema().hasField(lhs_predicate);
                bool rhs_valid = rhs_op->getOutputSchema().hasField(rhs_predicate);

                if (lhs_valid && rhs_valid) {
                    validPairs.push_back(pair);
                } else {
                    invalidPairs.push_back(pair);
                }
            }

            std::multimap<std::string, std::vector<std::pair<std::string, std::string>>> classifiedPredicates;
            classifiedPredicates.insert({"valid", validPairs});
            classifiedPredicates.insert({"invalid", invalidPairs});

            return classifiedPredicates;
        };

        // Filter predicates based on schema and classify them
        std::multimap<std::string, std::vector<std::pair<std::string, std::string>>> classified = filterValidPredicates(predicatePairs, pair.lhs, pair.rhs);

        // Extract valid and invalid predicates
        auto invalid_predicates = classified.find("invalid")->second;

        // If there are no invalid predicates, continue to the next pair
        if(invalid_predicates.size() == 0){
            refined_pairs.emplace_back(pair.lhs, pair.rhs, pair.lhs_predicate, pair.rhs_predicate);
            continue;
        }

        auto valid_predicates = classified.find("valid")->second;

        // Add refined pairs to the refined_pairs vector
        if (!valid_predicates.empty()) {
            // Rebuild the predicate strings
            for (int idx = 0; idx < valid_predicates.size(); idx++) {
                auto valid_pair = valid_predicates[idx];
                if (idx == 0) {
                    pair.lhs_predicate = valid_pair.first;
                    pair.rhs_predicate = valid_pair.second;
                } else {
                    pair.lhs_predicate += " AND " + valid_pair.first;
                    pair.rhs_predicate += " AND " + valid_pair.second;
                }
            }
            refined_pairs.emplace_back(pair.lhs, pair.rhs, pair.lhs_predicate, pair.rhs_predicate);
        }

        for(int idx = 0; idx < invalid_predicates.size(); idx++){
            Operator<B>* lhs = findJoinChildFromSqlInput(invalid_predicates[idx].first);
            Operator<B>* rhs = findJoinChildFromSqlInput(invalid_predicates[idx].second);

            // Create a new pair with the invalid predicates
            new_pairs.emplace_back(lhs, rhs, invalid_predicates[idx].first, invalid_predicates[idx].second);
        }
    }

    // Combine the original refined pairs with any new pairs formed
    join_pairs = std::move(refined_pairs);
    join_pairs.insert(join_pairs.end(), new_pairs.begin(), new_pairs.end());
}

template<typename B>
std::string BushyPlanEnumerator<B>::joinString(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::string result;
    for (const auto& str : strings) {
        if (!result.empty()) result += delimiter;
        result += str;
    }
    return result;
}


template<typename B>
void BushyPlanEnumerator<B>::addTransitivity(std::vector<JoinPair<B>>& join_pairs, JoinGraph<B>& joinGraph) {

    std::vector<JoinPair<B>> transitivePairs;

    // Helper function to split predicates into a set of conditions
    auto splitPredicates = [](const std::string& predicateInput1, const std::string& predicateInput2) -> std::set<std::string> {
        // Combine the two predicate inputs with " AND " between them if both are non-empty
        std::string combinedPredicate = predicateInput1;
        if (!predicateInput1.empty() && !predicateInput2.empty()) {
            combinedPredicate += " AND ";
        }
        combinedPredicate += predicateInput2;

        std::set<std::string> conditions;
        std::string delimiter = " AND ";
        size_t pos = 0;
        std::string token;

        while ((pos = combinedPredicate.find(delimiter)) != std::string::npos) {
            token = combinedPredicate.substr(0, pos); // Extract substring from beginning to delimiter
            conditions.insert(token); // Insert extracted condition
            combinedPredicate.erase(0, pos + delimiter.length()); // Erase processed part including the delimiter
        }
        // Insert the last or only condition
        if (!combinedPredicate.empty()) conditions.insert(combinedPredicate);

        return conditions;
    };

    for (auto& pair1 : join_pairs) {
        // Add nodes and edges to joinGraph
        joinGraph.addNode(pair1.lhs);
        joinGraph.addNode(pair1.rhs);
        joinGraph.addEdge(pair1.lhs, pair1.rhs, pair1.lhs_predicate, pair1.rhs_predicate);

        auto conditions1 = splitPredicates(pair1.lhs_predicate, pair1.rhs_predicate);

        for (auto& pair2 : join_pairs) {
            if (&pair1 == &pair2) continue;

            auto conditions2 = splitPredicates(pair2.lhs_predicate, pair2.rhs_predicate);

            // Find intersection of conditions to identify potential transitivity
            std::vector<std::string> commonConditions;
            std::set_intersection(conditions1.begin(), conditions1.end(),
                                  conditions2.begin(), conditions2.end(),
                                  std::back_inserter(commonConditions));

            if (!commonConditions.empty()) {
                bool pair1_overlapped = (pair1.lhs_predicate == commonConditions[0]);
                bool pair2_overlapped = (pair2.lhs_predicate == commonConditions[0]);

                // pair1.lhs, pair2.lhs are overlapped.
                if(pair1_overlapped && pair2_overlapped){
                    // Check if overlapping pair, pair1.lhs and pair2.lhs are the same op
                    if(pair1.lhs == pair2.lhs)
                        // Check if pair1.rhs and pair2.rhs are different
                        if(pair2.rhs != pair2.rhs)
                            addTransitivePairs(pair1.rhs, pair1.rhs_predicate, pair2.rhs, pair2.rhs_predicate, commonConditions, transitivePairs, joinGraph);
                }
                // pair1.lhs, pair2.rhs are overlapped.
                else if(pair1_overlapped && !pair2_overlapped){
                    // Check if overlapping pair, pair1.lhs and pair2.rhs are the same op
                    if(pair1.lhs == pair2.rhs)
                        // Check if pair1.rhs and pair2.lhs are different
                        if(pair1.rhs != pair2.lhs)
                            addTransitivePairs(pair1.rhs, pair1.rhs_predicate, pair2.lhs, pair2.lhs_predicate, commonConditions, transitivePairs, joinGraph);
                }
                // pair1.rhs, pair2.lhs are overlapped.
                else if(!pair1_overlapped && pair2_overlapped){
                    // Check if overlapping pair, pair1.rhs and pair2.lhs are the same op
                    if(pair1.rhs == pair2.lhs)
                        // Check if pair1.lhs and pair2.rhs are different
                        if(pair1.lhs != pair2.rhs)
                            addTransitivePairs(pair1.lhs, pair1.lhs_predicate, pair2.rhs, pair2.rhs_predicate, commonConditions, transitivePairs, joinGraph);
                }
                // pair1.rhs, pair2.rhs are overlapped.
                else if(!pair1_overlapped && !pair2_overlapped){
                    // Check if overlapping pair, pair1.rhs and pair2.rhs are the same op
                    if(pair1.rhs == pair2.rhs)
                        // Check if pair1.lhs and pair2.lhs are different
                        if(pair1.lhs != pair2.lhs)
                            addTransitivePairs(pair1.lhs, pair1.lhs_predicate, pair2.lhs, pair2.lhs_predicate, commonConditions, transitivePairs, joinGraph);
                }
            }
        }
    }

    // Incorporate transitive join pairs into the existing list of join pairs, each transitive join pairs, add edges to joinGraph
    for(auto& transitivePair : transitivePairs){
        join_pairs.push_back(transitivePair);
        joinGraph.addEdge(transitivePair.lhs, transitivePair.rhs, transitivePair.lhs_predicate, transitivePair.rhs_predicate);
    }
}

template<typename B>
void BushyPlanEnumerator<B>::addTransitivePairs(Operator<B>* pair1, const string pair1_predicate, Operator<B>* pair2, const string pair2_predicate, const std::vector<std::string> commonConditions, std::vector<JoinPair<B>>& transitivePairs, JoinGraph<B>& joinGraph){
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

    std::set<std::string> pair1_predicate_sets = splitPredicates(pair1_predicate);
    std::set<std::string> pair2_predicate_sets = splitPredicates(pair2_predicate);

    for(auto it = commonConditions.begin(); it != commonConditions.end(); ++it){
        // compare commonConditions with pair1_predicate and pair2_predicate after '_' for both string,
        // if not same, then remove the condition from that pair_predicate
        for (auto jp = pair1_predicate_sets.begin(); jp != pair1_predicate_sets.end();) {
            if (it->substr(it->find('_') + 1) != jp->substr(jp->find('_') + 1)) {
                // Erase and update the iterator to the next valid element
                jp = pair1_predicate_sets.erase(jp);
            } else {
                // Only increment the iterator if no erasure took place
                ++jp;
            }
        }

        // Repeat for pair2_predicate with the same logic
        for (auto jp = pair2_predicate_sets.begin(); jp != pair2_predicate_sets.end();) {
            if (it->substr(it->find('_') + 1) != jp->substr(jp->find('_') + 1)) {
                jp = pair2_predicate_sets.erase(jp);
            } else {
                ++jp;
            }
        }
    }

    // Before adding this to transitive pairs, check if the pair is already in the transitive pairs
    // If it is, then do not add it
    for(auto& transitivePair : transitivePairs){
        if(pair1 == transitivePair.lhs && pair2 == transitivePair.rhs)
            return;
        else if(pair1 == transitivePair.rhs && pair2 == transitivePair.lhs)
            return;
    }

    // Add to the list of transitive pairs
    transitivePairs.push_back(JoinPair<B>{pair1, pair2, joinPredicates(pair1_predicate_sets), joinPredicates(pair2_predicate_sets)});
}

template<typename B>
void BushyPlanEnumerator<B>::calculateCostsforJoinPairs(std::vector<JoinPair<B>> &joinPairs){
    for(JoinPair<B> pair : joinPairs){
        Operator<B> *lhs = pair.lhs;
        int lhs_id = lhs->getOperatorId();
        Operator<B> *rhs = pair.rhs;
        int rhs_id = rhs->getOperatorId();

        // get sql input operators costs from sql_input_ops with key is starting with 'op_id-'
        std::vector<Operator<B>*> lhs_sql_inputs = findEntriesWithPrefix(lhs_id);
        std::vector<Operator<B>*> rhs_sql_inputs = findEntriesWithPrefix(rhs_id);

        QuerySchema lhs_schema = lhs->getOutputSchema();
        QuerySchema rhs_schema = rhs->getOutputSchema();
        QuerySchema output_schema = QuerySchema::concatenate(lhs_schema, rhs_schema);
        QuerySchema swapped_output_schema = QuerySchema::concatenate(rhs_schema, lhs_schema);

        std::vector<uint32_t> lhs_predicates = convertPredicateToOrdinals(lhs_schema, pair.lhs_predicate);
        std::vector<uint32_t> rhs_predicates = convertPredicateToOrdinals(output_schema, pair.rhs_predicate);

        std::vector<uint32_t> swapped_lhs_predicates = convertPredicateToOrdinals(rhs_schema, pair.rhs_predicate);
        std::vector<uint32_t> swapped_rhs_predicates = convertPredicateToOrdinals(swapped_output_schema, pair.lhs_predicate);

        GenericExpression<B> *join_condition = FieldUtilities::getEqualityPredicate<B>(lhs_predicates, lhs_schema, rhs_predicates, rhs_schema);
        GenericExpression<B> *swapped_join_condition = FieldUtilities::getEqualityPredicate<B>(swapped_lhs_predicates, rhs_schema, swapped_rhs_predicates, lhs_schema);

        // Try SMJ(lhs, rhs), SMJ(rhs, lhs), NLJ(lhs, rhs), NLJ(rhs, lhs) and calculate the cost and store in the memoization table
        int fk_id = getFKId(pair.lhs_predicate, pair.rhs_predicate);

        for(auto it : lhs_sql_inputs){
            lhs->setSortOrder(it->getSortOrder());
            for(auto jt : rhs_sql_inputs){
                if(fk_id != -1) {
                    rhs->setSortOrder(jt->getSortOrder());
                    Operator<B> *lhs_sort_before_join = it->getSortOrder().size() != 0 ? new Sort<B>(lhs->clone(), it->getSortOrder()) : lhs->clone();
                    Operator<B> *rhs_sort_before_join = jt->getSortOrder().size() != 0 ? new Sort<B>(rhs->clone(), jt->getSortOrder()) : rhs->clone();

                    string memoization_key = addMemoizationKey(std::to_string(lhs_id), std::to_string(rhs_id));

                    // SMJ(lhs, rhs)
                    KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin<B>(lhs_sort_before_join, rhs_sort_before_join, fk_id, join_condition->clone());
                    smj->setOperatorId(next_op_id_);
                    // Insert cost of smj to memoization_table_
                    memoization_table_[0].insert(std::make_pair(memoization_key, subPlan<B>{smj, smj->planCost(), smj->getOutputOrderinString(), 1}));

                    // NLJ(lhs, rhs)
                    KeyedJoin<B> *nlj = new KeyedJoin<B>(lhs_sort_before_join->clone(), rhs_sort_before_join->clone(), fk_id, join_condition->clone());
                    nlj->setOperatorId(next_op_id_);
                    // Insert cost of nlj to memoization_table_
                    memoization_table_[0].insert(std::make_pair(memoization_key, subPlan<B>{nlj, nlj->planCost(), nlj->getOutputOrderinString(), 1}));

                    fk_id = 1 - fk_id;

                    // SMJ(rhs, lhs)
                    KeyedSortMergeJoin<B> *smj_swapped = new KeyedSortMergeJoin<B>(rhs_sort_before_join->clone(), lhs_sort_before_join->clone(), fk_id, swapped_join_condition->clone());
                    smj_swapped->setOperatorId(next_op_id_);
                    // Insert cost of smj to memoization_table_
                    memoization_table_[0].insert(std::make_pair(memoization_key, subPlan<B>{smj_swapped, smj_swapped->planCost(), smj_swapped->getOutputOrderinString(), 1}));

                    // NLJ(rhs, lhs)
                    KeyedJoin<B> *nlj_swapped = new KeyedJoin<B>(rhs_sort_before_join->clone(), lhs_sort_before_join->clone(), fk_id, swapped_join_condition->clone());
                    nlj_swapped->setOperatorId(next_op_id_);
                    // Insert cost of nlj to memoization_table_
                    memoization_table_[0].insert(std::make_pair(memoization_key, subPlan<B>{nlj_swapped, nlj_swapped->planCost(),nlj_swapped->getOutputOrderinString(), 1}));
                }
            }
        }
        next_op_id_++;
    }
}

template<typename B>
std::vector<uint32_t> BushyPlanEnumerator<B>::convertPredicateToOrdinals(const QuerySchema& schema, const std::string& predicates) {
    // Function to split predicates by " AND "
    auto splitPredicates = [](const std::string& predicate) -> std::vector<std::string> {
        std::vector<std::string> parts;
        std::string temp = predicate;
        size_t pos = 0;
        std::string delimiter = " AND ";
        while ((pos = temp.find(delimiter)) != std::string::npos) {
            parts.push_back(temp.substr(0, pos));
            temp.erase(0, pos + delimiter.length());
        }
        parts.push_back(temp); // Add the last or only part
        return parts;
    };

    std::vector<std::string> split_parts = splitPredicates(predicates);
    std::vector<uint32_t> ordinals;
    for (const auto& part : split_parts) {
        int ordinal = findFieldOrdinal(schema, part);
        ordinals.push_back(ordinal);
    }
    return ordinals;
}

template<typename B>
int BushyPlanEnumerator<B>::findFieldOrdinal(const QuerySchema& schema, const std::string& predicate) {
    for(auto it = schema.fields_.begin(); it != schema.fields_.end(); ++it) {
        if(it->second.getName() == predicate) {
            return it->first; // The key of the map entry is the ordinal
        }
    }
    return -1; // Indicate not found, or throw an exception if appropriate
}


template<typename B>
int BushyPlanEnumerator<B>::getFKId(const std::string& lhs_predicate, const std::string& rhs_predicate) {
    std::vector<std::string> PKList = {"n_nationkey", "r_regionkey", "s_suppkey", "c_custkey", "p_partkey", "o_orderkey", "l_linenumber"};

    // Helper function to split predicates by " AND "
    auto splitPredicates = [](const std::string& predicate) -> std::vector<std::string> {
        std::vector<std::string> parts;
        std::string temp = predicate;
        size_t pos = 0;
        std::string delimiter = " AND ";
        while ((pos = temp.find(delimiter)) != std::string::npos) {
            parts.push_back(temp.substr(0, pos));
            temp.erase(0, pos + delimiter.length());
        }
        parts.push_back(temp); // Add the last or only part
        return parts;
    };

    auto lhs_parts = splitPredicates(lhs_predicate);
    auto rhs_parts = splitPredicates(rhs_predicate);

    // Check if any part of lhs or rhs is in PKList
    bool lhs_found = std::any_of(lhs_parts.begin(), lhs_parts.end(), [&PKList](const std::string& part) {
        return std::find(PKList.begin(), PKList.end(), part) != PKList.end();
    });

    bool rhs_found = std::any_of(rhs_parts.begin(), rhs_parts.end(), [&PKList](const std::string& part) {
        return std::find(PKList.begin(), PKList.end(), part) != PKList.end();
    });

    // Determine FK relationship based on presence of parts in PKList
    if (lhs_found && !rhs_found)
        // lhs is PK, so rhs is FK.
        return 1;
    else if (!lhs_found && rhs_found)
        // rhs is PK, so lhs is FK.
        return 0;
    else
        // Either both are PKs/FKs, or neither part is a PK/FK relationship.
        return -1;
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
void BushyPlanEnumerator<B>::pickCheapJoinByGroup(int row_idx) {

    // Modified GroupKey to sort lhs and rhs op_id without considering their order
    using GroupKey = std::tuple<int, int, string>;
    std::map<GroupKey, subPlan<B>> groupedPlans;

    // Change the memoization_table_[row_idx] to a map with GroupKey as key
    for (auto &entry: memoization_table_[row_idx]) {
        const subPlan<B> &plan = entry.second;

        // Extract lhs and rhs op_ids
        int lhs_op_id = (plan.join->getChild(0)->getType() == OperatorType::SORT) ? plan.join->getChild(0)->getChild(0)->getOperatorId() : plan.join->getChild(0)->getOperatorId();
        int rhs_op_id = (plan.join->getChild(1)->getType() == OperatorType::SORT) ? plan.join->getChild(1)->getChild(0)->getOperatorId() : plan.join->getChild(1)->getOperatorId();

        // Ensure lhs_op_id is always <= rhs_op_id for consistent ordering
        if (lhs_op_id > rhs_op_id) {
            std::swap(lhs_op_id, rhs_op_id);
        }

        // Create the key with sorted op_ids and output_order_str
        GroupKey key(lhs_op_id, rhs_op_id, plan.output_order_str);

        auto it = groupedPlans.find(key);
        if (it == groupedPlans.end() || it->second.cost > plan.cost) {
            groupedPlans[key] = std::move(entry.second);
        }
    }

    // Replace memoization_table_[row_idx] with grouped entries
    memoization_table_[row_idx].clear();

    // get groupkey's first value

    for (auto &pair : groupedPlans) {
        memoization_table_[row_idx].insert(std::make_pair(addMemoizationKey(std::to_string(std::get<0>(pair.first)), std::to_string(std::get<1>(pair.first))), std::move(pair.second))); // Ensure move semantics are used
    }
}

template<typename B>
void BushyPlanEnumerator<B>::pickCheapJoinByGroup(std::multimap<std::string, subPlan<B>>& plan_candidates) {
    // Modified GroupKey to sort lhs and rhs op_id without considering their order
    using GroupKey = std::pair<string, string>;
    std::map<GroupKey, subPlan<B>> groupedPlans;

    // Change the memoization_table_[row_idx] to a map with GroupKey as key
    for (auto &entry: plan_candidates) {
        const subPlan<B> &plan = entry.second;

        // Create the key with sorted op_ids and output_order_str
        GroupKey key(entry.first, plan.output_order_str);

        auto it = groupedPlans.find(key);
        if (it == groupedPlans.end() || it->second.cost > plan.cost) {
            groupedPlans[key] = std::move(entry.second);
        }
    }

    // Replace memoization_table_[row_idx] with grouped entries
    plan_candidates.clear();

    // get groupkey's first value

    for (auto &pair : groupedPlans)
        plan_candidates.insert(std::make_pair(pair.first.first, pair.second));
}

template<typename B>
Operator<B>* BushyPlanEnumerator<B>::createBushyJoinPlan(const std::string hamiltonianPath) {
    std::istringstream iss(hamiltonianPath);
    std::string token;
    std::vector<std::tuple<int, std::string, std::string>> path;

    // Regular expression to match the pattern "id {predicate_from, predicate_to}"
    std::regex pathRegex(R"((\d+)\s*(?:\{\s*([^,]*),\s*([^}]*)\})?)"); // Matches "id" and optionally "{predicate_from, predicate_to}"
    std::smatch match;

    // Process each segment split by '-'
    while (std::getline(iss, token, '-')) {
        // Trim whitespace
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());

        // Search for operator ID and optional predicates
        if (std::regex_search(token, match, pathRegex)) {
            int operatorId = std::stoi(match[1].str());
            std::string fromPredicate = match.size() > 2 && !match[2].str().empty() ? match[2].str() : "N/A";
            std::string toPredicate = match.size() > 3 && !match[3].str().empty() ? match[3].str() : "N/A";
            path.emplace_back(operatorId, fromPredicate, toPredicate);
        }
    }

    std::vector<subPlanWithKey<B>> plans = formatJoinPlan(path);
    Operator<B>* cheapest_plan = nullptr;
    size_t min_cost = std::numeric_limits<size_t>::max();
    for(auto it : plans){
        if(min_cost > it.plan.cost){
            min_cost = it.plan.cost;
            cheapest_plan = it.plan.join.get();
        }
    }
    return cheapest_plan;
}

template<typename B>
std::vector<subPlanWithKey<B>> BushyPlanEnumerator<B>::formatJoinPlan(const std::vector<std::tuple<int, std::string, std::string>>& path) {
    if (path.size() == 1) {
        std::vector<Operator<B> *> ops = findEntriesWithPrefix(std::get<0>(path[0]));
        std::vector<subPlanWithKey<B>> result;
        for (auto op: ops) {
            subPlan<B> plan = {op, 0, op->getOutputOrderinString(), 0};
            result.push_back({std::to_string(op->getOperatorId()), plan});
        }
        return result;
    } else if (path.size() == 2) {
        string height_one_key =
                std::get<0>(path[0]) < std::get<0>(path[1]) ? "(" + std::to_string(std::get<0>(path[0])) + " JOIN " +
                                                              std::to_string(std::get<0>(path[1])) + ")" : "(" +
                                                                                                           std::to_string(
                                                                                                                   std::get<0>(
                                                                                                                           path[1])) +
                                                                                                           " JOIN " +
                                                                                                           std::to_string(
                                                                                                                   std::get<0>(
                                                                                                                           path[0])) +
                                                                                                           ")";

        // return subPlans that has key as height_one_key from memoization_table_[0]
        std::vector<subPlanWithKey<B>> result;
        for (auto it = memoization_table_[0].begin(); it != memoization_table_[0].end(); ++it) {
            if (it->first == height_one_key)
                result.push_back({height_one_key, it->second});
        }
        return result;
    } else {
        size_t mid = (path.size() + 1) / 2;
        std::vector<std::tuple<int, std::string, std::string>> left = std::vector<std::tuple<int, std::string, std::string>>(
                path.begin(), path.begin() + mid);
        string join_predicate_1 = std::get<1>(left.back());
        string join_predicate_2 = std::get<2>(left.back());

        auto left_candidates = formatJoinPlan(left);
        auto right_candidates = formatJoinPlan(
                std::vector<std::tuple<int, std::string, std::string>>(path.begin() + mid, path.end()));

        std::multimap<std::string, subPlan<B>> plan_candidates;
        int maximum_height;
        // Evaluate all combinations of joins
        for (auto &left: left_candidates) {
            QuerySchema lhs_schema = left.plan.join->getOutputSchema();
            int lhs_height = left.plan.height;
            for (auto &right: right_candidates) {
                QuerySchema rhs_schema = right.plan.join->getOutputSchema();
                maximum_height = std::max(lhs_height, right.plan.height);

                QuerySchema output_schema = QuerySchema::concatenate(lhs_schema, rhs_schema);
                QuerySchema swapped_output_schema = QuerySchema::concatenate(rhs_schema, lhs_schema);

                std::vector<uint32_t> lhs_predicates = convertPredicateToOrdinals(lhs_schema, join_predicate_1);
                std::vector<uint32_t> rhs_predicates = convertPredicateToOrdinals(output_schema, join_predicate_2);

                std::vector<uint32_t> swapped_lhs_predicates = convertPredicateToOrdinals(rhs_schema,
                                                                                          join_predicate_2);
                std::vector<uint32_t> swapped_rhs_predicates = convertPredicateToOrdinals(swapped_output_schema,
                                                                                          join_predicate_1);

                GenericExpression<B> *join_condition = FieldUtilities::getEqualityPredicate<B>(lhs_predicates,
                                                                                               lhs_schema,
                                                                                               rhs_predicates,
                                                                                               rhs_schema);
                GenericExpression<B> *swapped_join_condition = FieldUtilities::getEqualityPredicate<B>(
                        swapped_lhs_predicates, rhs_schema, swapped_rhs_predicates, lhs_schema);

                // Try SMJ(lhs, rhs), SMJ(rhs, lhs), NLJ(lhs, rhs), NLJ(rhs, lhs) and calculate the cost and store in the memoization table
                int fk_id = getFKId(join_predicate_1, join_predicate_2);

                if (fk_id != -1) {
                    // If, lhs / rhs is SQLInput, then we need to add sort after SQLInput

                    Operator<B> *lhs = left.plan.join->clone();
                    if (left.plan.join->getType() == OperatorType::SECURE_SQL_INPUT) {
                        Operator<B> *lhs_sort_before_join = new Sort<B>(lhs,
                                                                        left.plan.join->getSortOrder());
                        lhs = lhs_sort_before_join;
                    }
                    Operator<B> *rhs = right.plan.join->clone();
                    if (right.plan.join->getType() == OperatorType::SECURE_SQL_INPUT) {
                        Operator<B> *rhs_sort_before_join = new Sort<B>(rhs,
                                                                        right.plan.join->getSortOrder());
                        rhs = rhs_sort_before_join;
                    }

                    string memoization_key = addMemoizationKey(left.key, right.key);

                    std::cout << memoization_key << std::endl;

                    // SMJ(lhs, rhs)
                    KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin<B>(lhs, rhs,
                                                                           fk_id, join_condition->clone());
                    smj->setOperatorId(next_op_id_);
                    // Insert cost of smj to memoization_table_
                    plan_candidates.insert(std::make_pair(memoization_key, subPlan<B>{smj, smj->planCost(),
                                                                                      smj->getOutputOrderinString(),
                                                                                      maximum_height + 1}));

                    // NLJ(lhs, rhs)
                    KeyedJoin<B> *nlj = new KeyedJoin<B>(lhs->clone(), rhs->clone(),
                                                         fk_id, join_condition->clone());
                    nlj->setOperatorId(next_op_id_);
                    // Insert cost of nlj to memoization_table_
                    plan_candidates.insert(std::make_pair(memoization_key, subPlan<B>{nlj, nlj->planCost(),
                                                                                      nlj->getOutputOrderinString(),
                                                                                      maximum_height + 1}));

                    fk_id = 1 - fk_id;

                    // SMJ(rhs, lhs)
                    KeyedSortMergeJoin<B> *smj_swapped = new KeyedSortMergeJoin<B>(rhs->clone(),
                                                                                   lhs->clone(), fk_id,
                                                                                   swapped_join_condition->clone());
                    smj_swapped->setOperatorId(next_op_id_);
                    // Insert cost of smj to memoization_table_
                    plan_candidates.insert(std::make_pair(memoization_key,
                                                          subPlan<B>{smj_swapped, smj_swapped->planCost(),
                                                                     smj_swapped->getOutputOrderinString(),
                                                                     maximum_height + 1}));

                    // NLJ(rhs, lhs)
                    KeyedJoin<B> *nlj_swapped = new KeyedJoin<B>(rhs->clone(),
                                                                 lhs->clone(), fk_id,
                                                                 swapped_join_condition->clone());
                    nlj_swapped->setOperatorId(next_op_id_);
                    // Insert cost of nlj to memoization_table_
                    plan_candidates.insert(std::make_pair(memoization_key,
                                                          subPlan<B>{nlj_swapped, nlj_swapped->planCost(),
                                                                     nlj_swapped->getOutputOrderinString(),
                                                                     maximum_height + 1}));
                }
            }
        }
        next_op_id_++;

        printCalculatedValues(plan_candidates);
        pickCheapJoinByGroup(plan_candidates);
        std::cout << "After Group By " << std::endl;
        printCalculatedValues(plan_candidates);

        // Convert map to vector
        std::vector<subPlanWithKey<B>> final_candidates;

        // Ensure the vector is large enough to accommodate the maximum_height index
        if (maximum_height >= memoization_table_.size()) {
            memoization_table_.resize(maximum_height + 1);
        }

        for (auto &candidate: plan_candidates){
            final_candidates.push_back({candidate.first, candidate.second});
            memoization_table_[maximum_height].insert(std::make_pair(candidate.first, candidate.second));
        }
        return final_candidates;
    }
}

template<typename B>
void BushyPlanEnumerator<B>::printCalculatedValues(const std::multimap<std::string, subPlan<B>> plan_candidates) {
    for(auto& plan : plan_candidates){
        string type = "";
        if(plan.second.join->getType() == OperatorType::KEYED_SORT_MERGE_JOIN)
            type = "SortMergeJoin";
        else
            type = "NestedLoopJoin";

        if(plan.second.join->getChild(0)->getType() == OperatorType::SORT){
            std::cout << "LHS Index: " << plan.second.join->getChild(0)->getChild(0)->getOperatorId() << ", "
                      << " order by: " << DataUtilities::printSortDefinition(plan.second.join->getChild(0)->getSortOrder()) << ", ";
        }
        else {
            std::cout << "LHS Index: " << plan.second.join->getChild(0)->getOperatorId() << ", "
                      << " order by: "
                      << DataUtilities::printSortDefinition(plan.second.join->getChild(0)->getSortOrder()) << ", ";
        }
        if(plan.second.join->getChild(1)->getType() == OperatorType::SORT) {
            std::cout << "RHS Index: " << plan.second.join->getChild(1)->getChild(0)->getOperatorId() << ", "
                      << " order by: "
                      << DataUtilities::printSortDefinition(plan.second.join->getChild(1)->getSortOrder()) << ", ";
        }
        else{
            std::cout << "RHS Index: " << plan.second.join->getChild(1)->getOperatorId() << ", "
                      << " order by: "
                      << DataUtilities::printSortDefinition(plan.second.join->getChild(1)->getSortOrder()) << ", ";
        }
                  std::cout << "Type: " << type << ", "
                  << "Cost: " << plan.second.cost << ", "
                  << "Output Order: " << DataUtilities::printSortDefinition(plan.second.join->getSortOrder()) << ", "
                  << "Output Order String : " << plan.second.output_order_str << std::endl;
    }
}



template class vaultdb::BushyPlanEnumerator<bool>;
template class vaultdb::BushyPlanEnumerator<emp::Bit>;