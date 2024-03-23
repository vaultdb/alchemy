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

    // sort join_pairs with the output cardinality of join_pairs. put minimum card pair in the first
    /*
    sort(join_pairs)

    for pair in join_pairs:
            # recursively find all disjoint table pair
            disjoint_joins = find_join_of_disjoint_tables(pair)

            # if disjoint_joins are None, then do not create bushy plan. It is not possible to create bushy plan
            for join in disjoint_joins:
                    # if this pair & join are already considered previously, skip making plan.
                    if make_pair(pair, join) in considered_join_pairs:
                                continue
                    plan = create_bushy_plan(join, pair)
                    considered_join_pairs.add(make_pair(pair, join))
                    cost = calculate_cost(plan)
                    if cost < min_cost:
                                min_plan = plan
                                min_cost = cost
                                */

    std::map<int, Operator<B> *> sqlInputOps;
    auto left_deep_root = getRoot();

    // Step 1: Collect all SQLInput operations and map them by their operator ID
    collectSQLInputOps(left_deep_root, sqlInputOps);

    // Step 2: Recursively find join pairs starting from the left deep root
    findJoinPairs(left_deep_root, sqlInputOps);

    // Step 3 : Sort join_pairs with the output cardinality of join_pairs' 4th value(output cardainlity). Put minimum card in the first
    std::sort(join_pairs.begin(), join_pairs.end(), [](const JoinPairInfo<B> &a, const JoinPairInfo<B> &b) {
        return a.outputCardinality < b.outputCardinality;
    });

    size_t min_plan_cost_ = left_deep_root_->planCost();
    Operator<B> *min_cost_plan_ = left_deep_root_;

    // Step 4: Iterate through join pairs and create bushy plan
    for(auto it = join_pairs.begin(); it != join_pairs.end(); ++it) {
        JoinPairInfo<B> pair = *it;

        // Recursively find all disjoint table pairs
        auto disjoint_joins = findJoinOfDisjointTables(pair);

        // If disjoint_joins are None, then do not create bushy plan. It is not possible to create bushy plan
        if (disjoint_joins.empty()) {
            continue;
        }

        // Create bushy plan
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
                for(auto it = join_predicates.begin(); it != join_predicates.end(); ++it) {
                    string lhs_predicate = join_schema.fields_[it->first].getName();
                    string rhs_predicate = join_schema.fields_[it->second].getName();

                    auto lhs = findJoinChildFromSqlInput(lhs_predicate, sqlInputOps);
                    auto rhs = findJoinChildFromSqlInput(rhs_predicate, sqlInputOps);

                    auto join_info = getJoinType(lhs_predicate, rhs_predicate, lhs->getOutputCardinality(), rhs->getOutputCardinality());
                    join_pairs.push_back(JoinPairInfo<B>(std::make_pair(lhs, lhs_predicate), std::make_pair(rhs, rhs_predicate), join_info.first, join_info.second));
                }
            }
        }
        op = parent;
    }
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
std::vector<JoinPairInfo<B>> BushyPlanEnumerator<B>::findJoinOfDisjointTables(JoinPairInfo<B> selectedPair) {

    std::vector<JoinPairInfo<B>> disjointPairs;
    for (const auto& pair : join_pairs) {
        if (pair.lhs.first != selectedPair.lhs.first && pair.lhs.first != selectedPair.rhs.first &&
            pair.rhs.first != selectedPair.lhs.first && pair.rhs.first != selectedPair.rhs.first) {
            disjointPairs.push_back(pair);
        }
    }
    return disjointPairs;
}



template class vaultdb::BushyPlanEnumerator<bool>;
template class vaultdb::BushyPlanEnumerator<emp::Bit>;