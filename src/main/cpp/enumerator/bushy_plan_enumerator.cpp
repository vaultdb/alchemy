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

    // Step 1: Collect all SQLInput operations and map them by their operator ID
    // find join pairs from the left deep tree, and store them in join_pairs_vector
    std::vector<JoinPair<B>> join_pairs;
    collectSQLInputsAndJoinPairs(left_deep_root_, join_pairs);

    // Step 2 : Add transitivity to the join_pairs_ and create join graph
    addTransitivity();

    // Step 3 : Calcuate join cost for each join pair
    calculateCostsforJoinPairs(join_pairs);

    /*
    // Step 4 : Sort join_pairs_ with the output cardinality of join_pairs_' 4th value(output cardainlity). Put minimum card in the first
    std::sort(join_pairs_.begin(), join_pairs_.end(), [](const JoinPairInfo<B> &a, const JoinPairInfo<B> &b) {
        return a.outputCardinality < b.outputCardinality;
    });

    // Step 5 : Create join graph from the join_pairs_
    JoinGraph<B> joinGraph = createJoinGraph(sql_input_ops_, join_pairs_);

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

        // find the join child from the SQLInput operations
        for (auto& jp : join_predicates) {
            string lhs_predicate = join_schema.fields_[jp.first].getName();
            string rhs_predicate = join_schema.fields_[jp.second].getName();

            Operator<B> *lhs = op->getChild(0);
            Operator<B> *rhs = op->getChild(1);

            if (lhs->getType() != OperatorType::SECURE_SQL_INPUT)
                lhs = findJoinChildFromSqlInput(lhs_predicate);

            if (rhs->getType() != OperatorType::SECURE_SQL_INPUT)
                rhs = findJoinChildFromSqlInput(rhs_predicate);

            join_pairs.push_back(JoinPair<B>{lhs, rhs, lhs_predicate, rhs_predicate});
        }
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
void BushyPlanEnumerator<B>::calculateCostsforJoinPairs(std::vector<JoinPair<B>> &joinPairs){
    for(JoinPair<B> pair : joinPairs){
        Operator<B> *lhs = pair.lhs;
        string lhs_key = std::to_string(lhs->getOperatorId()) + '-';
        Operator<B> *rhs = pair.rhs;
        string rhs_key = std::to_string(rhs->getOperatorId()) + '-';

        // get sql input operators costs from sql_input_ops with key is starting with 'op_id-'
        std::vector<std::pair<std::string, inputRelation<B>>> lhs_sql_inputs = findEntriesWithPrefix(lhs_key);
        std::vector<std::pair<std::string, inputRelation<B>>> rhs_sql_inputs = findEntriesWithPrefix(rhs_key);

        for(auto it = lhs_sql_inputs.begin(); it != lhs_sql_inputs.end(); ++it){
            for(auto jt = rhs_sql_inputs.begin(); jt != rhs_sql_inputs.end(); ++jt){
                // Try SMJ(lhs, rhs), SMJ(rhs, lhs), NLJ(lhs, rhs), NLJ(rhs, lhs) and calculate the cost and store in the memoization table
                using boost::property_tree::ptree;
                ptree join_condition_tree = createJoinConditionTree(lhs->getOutputSchema(), rhs->getOutputSchema(), pair.lhs_predicate, pair.rhs_predicate);

                Expression<B> *join_condition = ExpressionParser<B>::parseExpression(join_condition_tree, lhs->getOutputSchema(), rhs->getOutputSchema());

                // SMJ(lhs, rhs)
                Operator<B> *smj = new KeyedSortMergeJoin<B>(lhs->clone(), rhs->clone(), 0, join_condition->clone());

                /*
                // check sort compatibility for SMJ

                // TODO : Need to check sort compatibility for SMJ
                if (smj->sortCompatible(lhs)) {
                    // If rhs is not scan, then add sort operator
                    // If rhs is scan, then do not add sort operator, just setSortOrder and updateCollation
                    if(rhs->getType() != OperatorType::SECURE_SQL_INPUT) {
                        // try inserting sort for RHS
                        auto join_key_idxs = smj->joinKeyIdxs();
                        // rhs will have second keys
                        vector<ColumnSort> rhs_sort;
                        int lhs_col_cnt = lhs->getOutputSchema().getFieldCount();
                        SortDefinition lhs_sort = lhs->getSortOrder();

                        for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                            int idx = join_key_idxs[i].second;
                            rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                        }

                        delete smj;

                        // Define sort before rhs_leaf
                        Operator<B> *rhs_sorter = new Sort<B>(rhs->clone(), rhs_sort);
                        smj = new KeyedSortMergeJoin<B>(lhs->clone(), rhs_sorter, kj->foreignKeyChild(),
                                                        kj->getPredicate()->clone());
                    }
                }
                else if (smj->sortCompatible(rhs)) {
                    // If lhs is not scan, then add sort operator
                    // If lhs is scan, then do not add sort operator, just setSortOrder and updateCollation
                    if(lhs->getType() != OperatorType::SECURE_SQL_INPUT) {
                        // add sort to lhs
                        // try inserting sort for RHS
                        auto join_key_idxs = smj->joinKeyIdxs();
                        // rhs will have second keys
                        vector<ColumnSort> lhs_sort;
                        SortDefinition rhs_sort = rhs->getSortOrder();

                        for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                            int idx = join_key_idxs[i].first;
                            lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                        }

                        delete smj;

                        // Define sort before lhs
                        Operator<B> *lhs_sorter = new Sort<B>(lhs->clone(), lhs_sort);
                        smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs->clone(), kj->foreignKeyChild(),
                                                        kj->getPredicate()->clone());
                        lhs_sorter->setParent(smj);
                    }
                }
                */
            }
        }

    }
}

template<typename B>
boost::property_tree::ptree BushyPlanEnumerator<B>::createJoinConditionTree(const QuerySchema& lhs, const QuerySchema& rhs, const string lhs_predicate, const string rhs_predicate) {
    using boost::property_tree::ptree;
    ptree join_condition_tree;

    // Construct the operation (op) part
    ptree op_tree;
    op_tree.put("name", "=");
    op_tree.put("kind", "EQUALS");
    op_tree.put("syntax", "BINARY");

    // Construct the operands part
    ptree operands_tree;
    ptree operand1, operand2;

    QuerySchema input_schema = QuerySchema::concatenate(lhs, rhs);

    // iterate input_schema and find the index of the lhs_predicate and rhs_predicate
    int lhs_ordinal = -1;
    int rhs_ordinal = -1;
    for(int i = 0; i < input_schema.getFieldCount(); i++){
        if(input_schema.fields_[i].getName() == lhs_predicate)
            lhs_ordinal = i;
        if(input_schema.fields_[i].getName() == rhs_predicate)
            rhs_ordinal = i;
    }

    // Set operands with the given ordinals
    operand1.put("input", lhs_ordinal);
    operand1.put("name", "$" + std::to_string(lhs_ordinal));

    operand2.put("input", rhs_ordinal);
    operand2.put("name", "$" + std::to_string(rhs_ordinal));

    operands_tree.push_back(std::make_pair("", operand1));
    operands_tree.push_back(std::make_pair("", operand2));

    // Assemble the condition tree
    join_condition_tree.put_child("op", op_tree);
    join_condition_tree.put_child("operands", operands_tree);

    return join_condition_tree;
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
void BushyPlanEnumerator<B>::findJoinOfDisjointTables(const JoinPairInfo<B>& initialPair, const JoinGraph<B>& joinGraph, std::vector<JoinPairInfo<B>>& currentDisjointPairs, bool isInitialCall) {
    if (isInitialCall) {
        currentDisjointPairs.clear();
        currentDisjointPairs.push_back(initialPair);
    }

    // Since getConnectedOperators now only returns a list of operators, we adjust accordingly.
    auto connectedOperators = joinGraph.getConnectedOperators(currentDisjointPairs);

    // Iterate over connected operators to find potential join pairs.
    for (Operator<B>* connectedOp : connectedOperators) {
        // Find disjoint pairs that includes connectedOp and are disjoint with currentDisjointPairs.
        std::vector<JoinPairInfo<B>> foundDisjointPairs = findCandidateDisjointPairs(connectedOp, currentDisjointPairs);

        // If no disjoint pairs were found, terminate the recursion.
        if (foundDisjointPairs.empty()) {
            // Compare and add missing inputs, finalize the plan.
            addMissingInputs(currentDisjointPairs);
            // Add the plan formed by newDisjointPairs to the list of potential plans.
            possible_disjoint_pairs_.push_back(currentDisjointPairs);
            continue;
        }

        // For each found disjoint pair, recurse or process further as needed.
        for (const auto& newPair : foundDisjointPairs) {
            std::vector<JoinPairInfo<B>> newDisjointPairs = currentDisjointPairs;
            newDisjointPairs.push_back(newPair);

            // Recursive call to explore further disjoint pairs from this new pair.
            findJoinOfDisjointTables(newPair, joinGraph, newDisjointPairs, false);
        }
    }
}

template<typename B>
std::vector<JoinPairInfo<B>> BushyPlanEnumerator<B>::findCandidateDisjointPairs(
        Operator<B>* connectedOp,
        const std::vector<JoinPairInfo<B>>& currentDisjointPairs) const
{
    std::vector<JoinPairInfo<B>> foundDisjointPairs;

    for (const auto& candidatePair : join_pairs_) {
        // Check if the candidatePair includes the connectedOp and is not already in currentDisjointPairs.
        if ((candidatePair.lhs.first == connectedOp || candidatePair.rhs.first == connectedOp) &&
            std::find(currentDisjointPairs.begin(), currentDisjointPairs.end(), candidatePair) == currentDisjointPairs.end()) {
            // Verify if the candidatePair is disjoint with respect to currentDisjointPairs.
            if (isDisjoint(candidatePair, currentDisjointPairs)) {
                foundDisjointPairs.push_back(candidatePair);
            }
        }
    }
    return foundDisjointPairs;
}

template<typename B>
bool BushyPlanEnumerator<B>::isDisjoint(const JoinPairInfo<B>& candidatePair, const std::vector<JoinPairInfo<B>>& currentDisjointPairs) const {
    // Iterate over currentDisjointPairs to check for any overlap with candidatePair.
    for (const auto& existingPair : currentDisjointPairs) {
        // Check if either side of the candidatePair matches either side of the existingPair.
        if (candidatePair.lhs.first == existingPair.lhs.first || candidatePair.lhs.first == existingPair.rhs.first ||
            candidatePair.rhs.first == existingPair.lhs.first || candidatePair.rhs.first == existingPair.rhs.first) {
            // If there's any overlap, the pair is not disjoint.
            return false;
        }
    }
    // If no overlaps were found, the candidatePair is considered disjoint.
    return true;
}

template<typename B>
void BushyPlanEnumerator<B>::addMissingInputs(std::vector<JoinPairInfo<B>>& currentDisjointPairs) {
    // Convert current disjoint pairs to a set of Operators to easily check for presence.
    std::set<Operator<B>*> presentOps;
//    for (const auto& pair : currentDisjointPairs) {
//        presentOps.insert(pair.lhs.first);
//        presentOps.insert(pair.rhs.first);
//    }
//
//    // Find the missing input operator by comparing with sql_input_ops.
//    for (const auto& entry : sql_input_ops_) {
//        Operator<B>* op = entry.second;
//
//        if (presentOps.find(op) == presentOps.end()) { // If op is missing
//            // Add the missing op. Since you mentioned at most one might be missing,
//            // we can handle it here. Determine how to add it (e.g., as a new pair with a dummy operator or attached to an existing pair).
//            // For simplicity, let's create a dummy operator and a new JoinPairInfo with the missing op.
//            Operator<B>* dummyOp = nullptr; // Assuming you have a way to create or specify a dummy operator.
//            char joinType = 'N'; // Example join type, adjust based on your schema.
//            size_t outputCardinality = 0; // Example cardinality, adjust as needed.
//
//            currentDisjointPairs.push_back(JoinPairInfo<B>(std::make_pair(op, ""), std::make_pair(dummyOp, ""), joinType, outputCardinality));
//            break; // Since at most one op might be missing, we can break after handling.
//        }
//    }
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