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
}

template<typename B>
void BushyPlanEnumerator<B>::createBushyBalancedTree() {

    // Step 1: Collect all SQLInput operations and map them by their operator ID
    // find join pairs from the left deep tree, and store them in join_pairs_vector
    std::vector<JoinPair<B>> join_pairs;
    collectSQLInputsAndJoinPairs(left_deep_root_, join_pairs);

    // Step 2 : Add transitivity to the join_pairs_ and create join graph
    JoinGraph<B> joinGraph;
    addTransitivity(join_pairs, joinGraph);

    // Step 3 : Calcuate join cost for each join pair
    calculateCostsforJoinPairs(join_pairs);

    for (const auto& plan : memoization_table_[0]) {
        string type = "";
        if(plan.type == OperatorType::KEYED_SORT_MERGE_JOIN)
            type = "SortMergeJoin";
        else
            type = "NestedLoopJoin";

        if (std::holds_alternative<SqlInputKey<B>>(plan.lhs.index) && std::holds_alternative<SqlInputKey<B>>(plan.rhs.index)) {
            std::cout << "LHS Index: " << std::get<SqlInputKey<B>>(plan.lhs.index).op_id << ", "
                      << " order by: " << DataUtilities::printSortDefinition(std::get<SqlInputKey<B>>(plan.lhs.index).sort_orders) << ", "
                      << "RHS Index: " << std::get<SqlInputKey<B>>(plan.rhs.index).op_id << ", "
                      << " order by: " << DataUtilities::printSortDefinition(std::get<SqlInputKey<B>>(plan.rhs.index).sort_orders) << ", "
                      << "Type: " << type << ", "
                      << "Cost: " << plan.cost << ", "
                      << "Output Order: " << DataUtilities::printSortDefinition(plan.output_order) << ", "
                      << "Output Order String : " << plan.output_order_str << std::endl;
        } else {
            // Handle the unexpected case where the index is not SqlInputKey<B>
            std::cerr << "Unexpected type in index. Expected SqlInputKey<B>." << std::endl;
        }
    }

    // Step 4 : group join pairs by their output sort order in memoization_table_[0]
    pickCheapJoinByGroup(0);

    // Step 5 : Find all paths from join graph.
    std::vector<string> hamiltonian_paths = joinGraph.findHamiltonianPaths();

    // Step 6 : Iterate hamiltonian path and create bushy plan
    // Prune the paths that are the same bushy plan (i.e., rhs lhs swapped)
    size_t min_plan_cost_ = left_deep_root_->planCost();
    Operator<B> *min_cost_plan_ = left_deep_root_;

    for(auto path : hamiltonian_paths){
        Operator<B>* plan = createBushyJoinPlan(path);
        size_t cost = plan->planCost();
        if(cost < min_plan_cost_){
            min_plan_cost_ = cost;
            min_cost_plan_ = plan;
        }
    }

    /*

    size_t min_plan_cost_ = left_deep_root_->planCost();
    Operator<B> *min_cost_plan_ = left_deep_root_;

    // Step 6: Iterate through join pairs and create bushy plan
    for(auto it = join_pairs_.begin(); it != join_pairs_.end(); ++it) {
        JoinPairInfo<B> pair = *it;

        std::vector<JoinPairInfo<B>> currentDisjointPairs;

        // Recursively find all disjoint table pairs
        findJoinOfDisjointTables(pair, joinGraph, currentDisjointPairs, true);

        // If disjoint_joins are None, then do not create bushy plan. It is not possible to create bushy plan
        if (possible_disjoint_pairs_.empty()) {
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
     */
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
            sql_input_ops_.insert(std::make_pair(SqlInputKey<B>(op_id, sort), inputRelation<B>(node, node->planCost())));
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
        joinGraph.addEdge(pair1.lhs, pair1.rhs);

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
        joinGraph.addEdge(transitivePair.lhs, transitivePair.rhs);
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
        std::vector<std::pair<SqlInputKey<B>, inputRelation<B>>> lhs_sql_inputs = findEntriesWithPrefix(lhs_id);
        std::vector<std::pair<SqlInputKey<B>, inputRelation<B>>> rhs_sql_inputs = findEntriesWithPrefix(rhs_id);

        for(auto it = lhs_sql_inputs.begin(); it != lhs_sql_inputs.end(); ++it){
            for(auto jt = rhs_sql_inputs.begin(); jt != rhs_sql_inputs.end(); ++jt){
                // Try SMJ(lhs, rhs), SMJ(rhs, lhs), NLJ(lhs, rhs), NLJ(rhs, lhs) and calculate the cost and store in the memoization table
                using boost::property_tree::ptree;
                ptree join_condition_tree = createJoinConditionTree(lhs->getOutputSchema(), rhs->getOutputSchema(), pair.lhs_predicate, pair.rhs_predicate);
                ptree swapped_join_condition_tree = createJoinConditionTree(rhs->getOutputSchema(), lhs->getOutputSchema(), pair.rhs_predicate, pair.lhs_predicate);

                Expression<B> *join_condition = ExpressionParser<B>::parseExpression(join_condition_tree, lhs->getOutputSchema(), rhs->getOutputSchema());
                Expression<B> *swapped_join_condition = ExpressionParser<B>::parseExpression(swapped_join_condition_tree, rhs->getOutputSchema(), lhs->getOutputSchema());

                int fk_id = getFKId(pair.lhs_predicate, pair.rhs_predicate);

                if(fk_id != -1) {

                    Operator<B> *lhs_sorted = lhs->clone();
                    lhs_sorted->setSortOrder(it->first.sort_orders);
                    Operator<B> *rhs_sorted = rhs->clone();
                    rhs_sorted->setSortOrder(jt->first.sort_orders);

                    // SMJ(lhs, rhs)
                    KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin<B>(lhs_sorted, rhs_sorted, fk_id, join_condition->clone());
                    // Insert cost of smj to memoization_table_
                    memoization_table_[0].push_back(
                            subPlan<B>{it->first, jt->first, OperatorType::KEYED_SORT_MERGE_JOIN, smj->planCost(),
                                       smj->getSortOrder(), smj->getOutputOrderinString()});

                    // NLJ(lhs, rhs)
                    KeyedJoin<B> *nlj = new KeyedJoin<B>(lhs_sorted->clone(), rhs_sorted->clone(), fk_id, join_condition->clone());
                    // Insert cost of nlj to memoization_table_
                    memoization_table_[0].push_back(
                            subPlan<B>{it->first, jt->first, OperatorType::KEYED_NESTED_LOOP_JOIN, nlj->planCost(),
                                       nlj->getSortOrder(), nlj->getOutputOrderinString()});

                    fk_id = 1 - fk_id;

                    // SMJ(rhs, lhs)
                    KeyedSortMergeJoin<B> *smj_swapped = new KeyedSortMergeJoin<B>(rhs_sorted->clone(), lhs_sorted->clone(), fk_id, swapped_join_condition->clone());
                    // Insert cost of smj to memoization_table_
                    memoization_table_[0].push_back(
                            subPlan<B>{jt->first, it->first, OperatorType::KEYED_SORT_MERGE_JOIN,
                                       smj_swapped->planCost(), smj_swapped->getSortOrder(), smj_swapped->getOutputOrderinString()});

                    // NLJ(rhs, lhs)
                    KeyedJoin<B> *nlj_swapped = new KeyedJoin<B>(rhs_sorted->clone(), lhs_sorted->clone(), fk_id, swapped_join_condition->clone());
                    // Insert cost of nlj to memoization_table_
                    memoization_table_[0].push_back(
                            subPlan<B>{jt->first, it->first, OperatorType::KEYED_NESTED_LOOP_JOIN,
                                       nlj_swapped->planCost(), nlj_swapped->getSortOrder(), nlj_swapped->getOutputOrderinString()});

                    // TODO : Need to think we need to keep those created Join to memoization_table_ or not.
                    // IF not, then need to delete those created join.
                }
            }
        }

    }
}

template<typename B>
boost::property_tree::ptree BushyPlanEnumerator<B>::createJoinConditionTree(const QuerySchema& lhs, const QuerySchema& rhs, const string& lhs_predicate, const string& rhs_predicate) {
    using boost::property_tree::ptree;
    ptree join_condition_tree;

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

    // Check and split the lhs_predicate and rhs_predicate
    auto lhs_parts = splitPredicates(lhs_predicate);
    auto rhs_parts = splitPredicates(rhs_predicate);

    // Ensure both parts have equal size, otherwise, it's a malformed condition
    if (lhs_parts.size() != rhs_parts.size()) {
        throw std::runtime_error("Mismatch in number of predicates.");
    }

    QuerySchema input_schema = QuerySchema::concatenate(lhs, rhs);

    if (lhs_parts.size() == 1) { // Handle simple condition
        return createSimpleConditionTree(input_schema, lhs_parts[0], rhs_parts[0]);
    } else { // Handle composite condition
        ptree and_tree;
        and_tree.put("name", "AND");
        and_tree.put("kind", "AND");
        and_tree.put("syntax", "BINARY");

        ptree operands_tree;

        for (size_t i = 0; i < lhs_parts.size(); ++i) {
            operands_tree.push_back(std::make_pair("", createSimpleConditionTree(input_schema, lhs_parts[i], rhs_parts[i])));
        }

        and_tree.add_child("operands", operands_tree);
        join_condition_tree.add_child("condition", and_tree);

        return join_condition_tree;
    }
}

template<typename B>
boost::property_tree::ptree BushyPlanEnumerator<B>::createSimpleConditionTree(const QuerySchema& input_schema, const std::string& lhs_predicate, const std::string& rhs_predicate) {
    using boost::property_tree::ptree;
    ptree condition_tree;

    // Similar construction to your original function for EQUALS condition
    ptree op_tree;
    op_tree.put("name", "=");
    op_tree.put("kind", "EQUALS");
    op_tree.put("syntax", "BINARY");

    ptree operands_tree;
    ptree operand1, operand2;

    // Find the index of lhs_predicate and rhs_predicate in input_schema
    int lhs_ordinal = findFieldOrdinal(input_schema, lhs_predicate);
    int rhs_ordinal = findFieldOrdinal(input_schema, rhs_predicate);

    operand1.put("input", lhs_ordinal);
    operand1.put("name", "$" + std::to_string(lhs_ordinal));
    operand2.put("input", rhs_ordinal);
    operand2.put("name", "$" + std::to_string(rhs_ordinal));

    operands_tree.push_back(std::make_pair("", operand1));
    operands_tree.push_back(std::make_pair("", operand2));

    condition_tree.put_child("op", op_tree);
    condition_tree.put_child("operands", operands_tree);

    return condition_tree;
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
    // Helper lambda to extract op_id
    auto getOpId = [](const auto& index) -> int {
        return std::visit(overloaded{
                [](const SqlInputKey<B>& key) { return key.op_id; },
                [](const std::pair<int, int>&) { return -1; }
        }, index);
    };

    // Modified GroupKey to sort lhs and rhs op_id without considering their order
    using GroupKey = std::tuple<int, int, string>;

    auto comparator = [](const GroupKey& a, const GroupKey& b) -> bool {
        return a < b; // Tuple provides a lexicographical comparison by default
    };

    std::map<GroupKey, subPlan<B>*> groupedPlans;

    // Change the memoization_table_[row_idx] to a map with GroupKey as key
    for (auto& plan : memoization_table_[row_idx]) {
        // Extract lhs and rhs op_ids
        int lhs_op_id = getOpId(plan.lhs.index);
        int rhs_op_id = getOpId(plan.rhs.index);

        // Ensure lhs_op_id is always <= rhs_op_id for consistent ordering
        if (lhs_op_id > rhs_op_id) {
            std::swap(lhs_op_id, rhs_op_id);
        }

        // Create the key with sorted op_ids and output_order_str
        GroupKey key = std::make_tuple(lhs_op_id, rhs_op_id, plan.output_order_str);
        auto it = groupedPlans.find(key);
        if (it == groupedPlans.end() || it->second->cost > plan.cost) {
            groupedPlans[key] = &plan; // Update to point to the lower-cost plan
        }
    }
    // Replace memoization_table_[row_idx] with grouped entries
    memoization_table_[row_idx].clear();
    for (const auto& pair : groupedPlans) {
        // Dereference the stored pointer to add the object back to the vector
        memoization_table_[row_idx].push_back(*(pair.second));
    }
}

template<typename B>
Operator<B>* BushyPlanEnumerator<B>::createBushyJoinPlan(const std::string hamiltonianPath) {
    std::vector<int> path;
    std::istringstream iss(hamiltonianPath);
    std::string node;

    // Split the string by '-' and convert to integers
    while (std::getline(iss, node, '-')) {
        path.push_back(std::stoi(node));
    }
}


template class vaultdb::BushyPlanEnumerator<bool>;
template class vaultdb::BushyPlanEnumerator<emp::Bit>;