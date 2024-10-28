#include "opt/plan_optimizer.h"
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

#include <util/logger.h>
#include <regex>

using namespace vaultdb;
using boost::property_tree::ptree;
using namespace Logging;

template<typename B>
PlanOptimizer<B>::PlanOptimizer(Operator<B> * root, map<int, Operator<B> * > operators, vector<Operator<B> * > support_ops, map<int, vector<SortDefinition>> interesting_orders){
    root_ = root;
    root_->setParent(nullptr);
    operators_ = operators;
    support_ops_ = support_ops;
    interesting_sort_orders_ = interesting_orders;
}


// optimize a left-deep tree with a fixed join order
// optimize operator implementations and sort placement

// Plan enumerator to iteratively try all potential impls for each operator declared in query plan.
//  * For leafs/LogicalValues cycle through all interesting sort orders then recurse to next tree layer
//  * For joins cycle through NLJ vs SMJ (assume no partial sort opt for now)
//  * For aggs try NLA and SMA (if a card bound exists)
//  * For SMA check if we need sort before op.
//  * After SMA assess if we still need any sort that's a parent to it.
//  * Important : Since we get leaf from operators_.begin, json file needs to be in the increasing order
//  * For example, if OP#0, OP#1 are logical values, OP#2 join should have lhs as 0, and rhs as 1, not vice versa.

template<typename B>
Operator<B> *PlanOptimizer<B>::optimizeTree() {
    // Clone tree to new_root
    auto new_root = root_->clone();
    auto leaf = new_root;
    while(leaf->getChild() != nullptr)
        leaf = leaf->getChild();

    assert(leaf->getType() == OperatorType::SECURE_SQL_INPUT);

    optimizeTreeHelper(leaf);

    std::cout << "Total Plans : " << std::to_string(total_plan_cnt_) << endl;

    delete new_root;

    // Delete the elements in operators_
    for(auto& pair : operators_) {
        pair.second->setChild(nullptr);
        pair.second->setChild(nullptr, 1);
        delete pair.second; // delete the Operator<B> pointer
    }
    operators_.clear();

    // Delete the elements in support_ops_
    for(auto& op : support_ops_) {
        op->setChild(nullptr);
        op->setChild(nullptr, 1);
        delete op; // delete the Operator<B> pointer
    }
    support_ops_.clear();
    root_ = nullptr;

    return min_cost_plan_;
}

// helper function for recursing up operator tree
template<typename B>
void PlanOptimizer<B>::optimizeTreeHelper(Operator<B> *op) {

    Operator<B> *node = op->clone();

    switch(op->getType()) {
        case OperatorType::SQL_INPUT:
        case OperatorType::ZK_SQL_INPUT:
        case OperatorType::TABLE_INPUT: {
            // first try with given sort order (if any) and empty set unconditionally
            auto sorts = getCollations(op);
            for (auto &sort: sorts) {
                node->setSortOrder(sort);
                recurseNode(node);
            }
            break;
        }
        case OperatorType::SECURE_SQL_INPUT:{
            // first try with given sort order (if any) and empty set unconditionally
            SecureSqlInput* secureSqlInputOp = dynamic_cast<SecureSqlInput*>(op);

            // If it is party scan, just run the node with default sort order
            if(secureSqlInputOp->getInputParty() > 0)
                recurseNode(node);
            else {
                auto sorts = getCollations(op);
                for (auto &sort: sorts) {
                    node = op->clone();
                    node->setSortOrder(sort);
                    recurseNode(node);
                }
            }
            break;
        }
        case OperatorType::KEYED_NESTED_LOOP_JOIN:
        case OperatorType::KEYED_SORT_MERGE_JOIN: {
            // for each rhs leaf collation cycle through its collations and then try both SMJ and NLJ with each collation
            recurseJoin(node);
            break;
        }
        case OperatorType::NESTED_LOOP_AGGREGATE:
        case OperatorType::SORT_MERGE_AGGREGATE: {
            // for each agg try NLA and SMA
            recurseAgg(node);
            break;
        }
        case OperatorType::SORT:
            // Sort is always same with sort order
            recurseSort(node);
            break;
        default:
            // Same with child's sort order
            node->updateCollation();
            recurseNode(node);
            break;
    }
    delete node;
}


template<typename B>
void PlanOptimizer<B>::recurseNode(Operator<B> *op) {
    int op_id = op->getOperatorId();
    Operator<B> *parent = operators_[op_id]->getParent();

    // If there is sort for join or aggregate, then id will be -1. skip it
    if(parent != nullptr) {
        if (parent->getOperatorId() < 0)
            parent = parent->getParent();

        // parent is pointing original child with original sort order
        // Need to repoint to op

        // Should make it no update collation because we will update collation at recurseNode() function.
        // Need to handle this case because if updateCollation then SortMergeAggregate will occurs error in the updateCollation's SortCompatible
        parent->setChildWithNoUpdateCollation(op);

        if(parent->getType() == OperatorType::SORT_MERGE_AGGREGATE){
            SortMergeAggregate<B> *sma = (SortMergeAggregate<B> *)parent;
            sma->is_optimized_cloned_ = true;
        }

    }


    // at the root node
    if(parent == nullptr) {
        size_t plan_cost = op->planCost();
//        std::cout << op->printTree() << endl;
//        std::cout << "Cost : " << std::to_string(plan_cost) << "\n" << "----------------------------" << std::endl;
        total_plan_cnt_++;

        if(plan_cost < min_plan_cost_) {
            min_plan_cost_ = plan_cost;
            if(min_cost_plan_ != nullptr)
                delete min_cost_plan_;
            min_cost_plan_ = op->clone();
        }
        return;
    }

    // else recurse up the tree
    optimizeTreeHelper(parent);

}


template<typename B>
Operator<B> *PlanOptimizer<B>::fetchLeaf(Operator<B> *op) {
    while(!op->isLeaf()) {
        op = op->getChild();
    }
    return op;
}

template<typename B>
void PlanOptimizer<B>::recurseJoin(Operator<B> *join) {
    auto lhs = join->getChild();
    auto rhs = join->getChild(1);

    if (rhs->getType() == OperatorType::PROJECT) {
        if (rhs->getChild()->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN ||
            rhs->getChild()->getType() == OperatorType::KEYED_SORT_MERGE_JOIN) {

            auto right_join = rhs->getChild();

            auto right_join_lhs = right_join->getChild();
            auto right_join_rhs = right_join->getChild(1);

            auto rhs_sorts = getCollations(right_join_rhs);
            auto lhs_sorts = getCollations(right_join_lhs);

            for (auto &l_sort: lhs_sorts) {
                for (auto &sort: rhs_sorts) {
                    // Clone the entire left and right subtrees
                    Operator<B> *right_join_lhs_clone = right_join_lhs->clone();
                    Operator<B> *right_join_rhs_clone = right_join_rhs->clone();

                    // Apply the sort order to the right child
                    right_join_lhs_clone->setSortOrder(l_sort);
                    right_join_rhs_clone->setSortOrder(sort);

                    // If join was NLJ
                    if (right_join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {

                        // Create plan for NLJ first.
                        KeyedJoin<B> *kj = (KeyedJoin<B> *) right_join->clone();
                        kj->setChild(right_join_lhs_clone, 0);
                        kj->setChild(right_join_rhs_clone, 1);
                        kj->updateCollation();

                        int op_id = kj->getOperatorId();
                        Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                        updated_rhs->setChild(kj);
                        updated_rhs->updateCollation();
                        Operator<B> *lhs_clone = lhs->clone();
                        Operator<B> *rhs_clone = updated_rhs->clone();
                        enumerateJoin(join, lhs_clone, rhs_clone);


                        // Create plan for SMJ next.
                        // Default, we set the SMJ that has lhs, rhs all sort compatible.
                        KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin(right_join_lhs_clone->clone(), right_join_rhs_clone->clone(),
                                                                            kj->foreignKeyChild(), kj->getPredicate()->clone());

                        // Check sort compatibility for SMJ
                        // If Only right_join_lhs_clone is sort compatible
                        if (smj->sortCompatible(right_join_lhs_clone) && !smj->sortCompatible(right_join_rhs_clone) ) {
                                auto join_key_idxs = smj->joinKeyIdxs();
                                vector<ColumnSort> rhs_sort;
                                int lhs_col_cnt = right_join_lhs_clone->getOutputSchema().getFieldCount();
                                SortDefinition lhs_sort = right_join_lhs_clone->getSortOrder();

                                for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                    int idx = join_key_idxs[i].second;
                                    rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                                }

                                delete smj;

                                Operator<B> *rhs_sorter = new Sort<B>(right_join_rhs_clone->clone(), rhs_sort);
                                smj = new KeyedSortMergeJoin<B>(right_join_lhs_clone->clone(), rhs_sorter, kj->foreignKeyChild(),
                                                                kj->getPredicate()->clone());
                        }
                        // Else if Only right_join_rhs_clone is sort compatible
                        else if (smj->sortCompatible(right_join_rhs_clone) && !smj->sortCompatible(right_join_lhs_clone)) {
                                auto join_key_idxs = smj->joinKeyIdxs();
                                vector<ColumnSort> lhs_sort;
                                SortDefinition rhs_sort = right_join_rhs_clone->getSortOrder();

                                for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                    int idx = join_key_idxs[i].first;
                                    lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                                }

                                delete smj;

                                Operator<B> *lhs_sorter = new Sort<B>(right_join_lhs_clone->clone(), lhs_sort);
                                smj = new KeyedSortMergeJoin<B>(lhs_sorter, right_join_rhs_clone->clone(), kj->foreignKeyChild(),
                                                                kj->getPredicate()->clone());
                                lhs_sorter->setParent(smj);
                        }
                        // Else if both are not sort compatible
                        else if (!smj->sortCompatible(right_join_rhs_clone) && !smj->sortCompatible(right_join_lhs_clone)) {
                            auto join_key_idxs = smj->joinKeyIdxs();

                            // Sort the lhs according to the rhs sort order
                            vector<ColumnSort> lhs_sort;
                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].first;
                                lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                            }
                            int lhs_col_cnt = right_join_lhs_clone->getOutputSchema().getFieldCount();

                            // Sort the rhs according to the lhs sort order
                            vector<ColumnSort> rhs_sort;
                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].second;
                                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                            }

                            delete smj;

                            // Apply sorting to both lhs and rhs
                            Operator<B> *lhs_sorter = new Sort<B>(right_join_lhs_clone->clone(), lhs_sort);
                            Operator<B> *rhs_sorter = new Sort<B>(right_join_rhs_clone->clone(), rhs_sort);

                            // Create a new SortMergeJoin after sorting both lhs and rhs
                            smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, kj->foreignKeyChild(), kj->getPredicate()->clone());

                            lhs_sorter->setParent(smj);
                            rhs_sorter->setParent(smj);
                        }

                        smj->setOperatorId(kj->getOperatorId());
                        smj->updateCollation();
                        op_id = smj->getOperatorId();
                        updated_rhs = operators_[op_id]->getParent()->clone();
                        updated_rhs->setChild(smj);
                        updated_rhs->updateCollation();
                        lhs_clone = lhs->clone();
                        rhs_clone = updated_rhs->clone();
                        enumerateJoin(join, lhs_clone, rhs_clone);

                        delete smj;
                        delete kj;

                    }
                    // If join was SMJ
                    else if (right_join->getType() == OperatorType::KEYED_SORT_MERGE_JOIN) {

                        // Create plan for SMJ first
                        KeyedSortMergeJoin<B> *smj = (KeyedSortMergeJoin<B> *) right_join->clone();
                        smj->setChild(right_join_lhs_clone, 0);
                        smj->setChild(right_join_rhs_clone, 1);
                        smj->updateCollation();

                        int foreign_key = smj->foreignKeyChild();
                        int operatorId = smj->getOperatorId();

                        if (smj->sortCompatible(right_join_lhs_clone)) {
                            auto join_key_idxs = smj->joinKeyIdxs();
                            vector<ColumnSort> rhs_sort;
                            int lhs_col_cnt = right_join_lhs_clone->getOutputSchema().getFieldCount();
                            SortDefinition lhs_sort = right_join_lhs_clone->getSortOrder();

                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].second;
                                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                            }

                            if (!smj->sortCompatible(right_join_rhs_clone)) {
                                Operator<B> *rhs_sorter = new Sort<B>(right_join_rhs_clone->clone(), rhs_sort);
                                Operator<B> *new_smj = new KeyedSortMergeJoin<B>(right_join_lhs_clone->clone(), rhs_sorter, foreign_key,
                                                                                 smj->getPredicate()->clone());
                                new_smj->setOperatorId(operatorId);
                                new_smj->updateCollation();

                                int op_id = new_smj->getOperatorId();
                                Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                                updated_rhs->setChild(new_smj);
                                updated_rhs->updateCollation();
                                Operator<B> *lhs_clone = lhs->clone();
                                Operator<B> *rhs_clone = updated_rhs->clone();
                                enumerateJoin(join, lhs_clone, rhs_clone);

                                delete new_smj;
                            } else {
                                smj->getChild(1)->setSortOrder(rhs_sort);
                                smj->updateCollation();

                                int op_id = smj->getOperatorId();
                                Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                                updated_rhs->setChild(smj);
                                updated_rhs->updateCollation();
                                Operator<B> *lhs_clone = lhs->clone();
                                Operator<B> *rhs_clone = updated_rhs->clone();
                                enumerateJoin(join, lhs_clone, rhs_clone);
                            }
                        }
                        else if (smj->sortCompatible(right_join_rhs_clone)) {
                            auto join_key_idxs = smj->joinKeyIdxs();
                            vector<ColumnSort> lhs_sort;
                            SortDefinition rhs_sort = right_join_rhs_clone->getSortOrder();

                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].first;
                                lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                            }

                            //if (right_join_lhs_clone->getType() != OperatorType::SECURE_SQL_INPUT) {
                            if (!smj->sortCompatible(right_join_lhs_clone)) {
                                Operator<B> *lhs_sorter = new Sort<B>(right_join_lhs_clone->clone(), lhs_sort);
                                Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_sorter, right_join_rhs_clone->clone(), foreign_key,
                                                                                 smj->getPredicate()->clone());
                                new_smj->setOperatorId(operatorId);
                                new_smj->updateCollation();

                                int op_id = new_smj->getOperatorId();
                                Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                                updated_rhs->setChild(new_smj);
                                updated_rhs->updateCollation();
                                Operator<B> *lhs_clone = lhs->clone();
                                Operator<B> *rhs_clone = updated_rhs->clone();
                                enumerateJoin(join, lhs_clone, rhs_clone);
                            } else {
                                smj->getChild()->setSortOrder(lhs_sort);
                                smj->updateCollation();

                                int op_id = smj->getOperatorId();
                                Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                                updated_rhs->setChild(smj);
                                updated_rhs->updateCollation();
                                Operator<B> *lhs_clone = lhs->clone();
                                Operator<B> *rhs_clone = updated_rhs->clone();
                                enumerateJoin(join, lhs_clone, rhs_clone);
                            }
                        }
                        else if (!smj->sortCompatible(right_join_lhs_clone) && !smj->sortCompatible(right_join_rhs_clone)) {
                            auto join_key_idxs = smj->joinKeyIdxs();

                            // Sort the lhs according to the rhs sort order
                            vector<ColumnSort> lhs_sort;
                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].first;
                                lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
                            }
                            int lhs_col_cnt = right_join_lhs_clone->getOutputSchema().getFieldCount();

                            // Sort the rhs according to the lhs sort order
                            vector<ColumnSort> rhs_sort;
                            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                                int idx = join_key_idxs[i].second;
                                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                            }

                            // Apply sorting to both lhs and rhs
                            Operator<B> *lhs_sorter = new Sort<B>(right_join_lhs_clone->clone(), lhs_sort);
                            Operator<B> *rhs_sorter = new Sort<B>(right_join_rhs_clone->clone(), rhs_sort);

                            // Create a new SortMergeJoin after sorting both lhs and rhs
                            Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, foreign_key, smj->getPredicate()->clone());
                            new_smj->updateCollation();
                            new_smj->setOperatorId(operatorId);
                            lhs_sorter->setParent(new_smj);
                            rhs_sorter->setParent(new_smj);

                            int op_id = new_smj->getOperatorId();
                            Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                            updated_rhs->setChild(new_smj);
                            updated_rhs->updateCollation();
                            Operator<B> *lhs_clone = lhs->clone();
                            Operator<B> *rhs_clone = updated_rhs->clone();
                            enumerateJoin(join, lhs_clone, rhs_clone);

                            delete new_smj;

                        }
                        else {
                            smj->updateCollation();

                            int op_id = smj->getOperatorId();
                            Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                            updated_rhs->setChild(smj);
                            updated_rhs->updateCollation();
                            Operator<B> *lhs_clone = lhs->clone();
                            Operator<B> *rhs_clone = updated_rhs->clone();
                            enumerateJoin(join, lhs_clone, rhs_clone);
                        }

                        // Create plan for NLJ next.
                        KeyedJoin<B> *kj = new KeyedJoin(right_join_lhs_clone->clone(), right_join_rhs_clone->clone(), foreign_key,
                                                         smj->getPredicate()->clone());
                        kj->setOperatorId(smj->getOperatorId());
                        kj->updateCollation();

                        int op_id = kj->getOperatorId();
                        Operator<B> *updated_rhs = operators_[op_id]->getParent()->clone();
                        updated_rhs->setChild(kj);
                        updated_rhs->updateCollation();
                        Operator<B> *lhs_clone = lhs->clone();
                        Operator<B> *rhs_clone = updated_rhs->clone();
                        enumerateJoin(join, lhs_clone, rhs_clone);

                        delete kj;
                        delete smj;
                    }
                }
            }
        }
    }
    else {
        auto rhs_sorts = getCollations(rhs);
        auto lhs_sorts = getCollations(lhs);

        for (auto &sort: rhs_sorts) {

            Operator<B> *rhs_clone = rhs->clone();
            Operator<B> *lhs_clone = lhs->clone();

            // Apply the sort order to the right child
            rhs_clone->setSortOrder(sort);

            enumerateJoin(join, lhs_clone, rhs_clone);
        }
    }
}

template<typename B>
void PlanOptimizer<B>::enumerateJoin(Operator<B>* join, Operator<B>* lhs_clone, Operator<B>* rhs_clone) {
    int operatorId = join->getOperatorId();

    if (join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {
        // Create plan for NLJ first
        KeyedJoin<B> *kj = (KeyedJoin<B> *) join->clone();
        kj->setChild(lhs_clone, 0);
        kj->setChild(rhs_clone, 1);
        kj->updateCollation();

        recurseNode(kj);

        // Create plan for SMJ next.
        KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin(lhs_clone->clone(), rhs_clone->clone(),
                                                            kj->foreignKeyChild(), kj->getPredicate()->clone());

        // check sort compatibility for SMJ
        if (smj->sortCompatible(lhs_clone) && !smj->sortCompatible(rhs_clone)) {
                auto join_key_idxs = smj->joinKeyIdxs();
                vector<ColumnSort> rhs_sort;
                int lhs_col_cnt = lhs_clone->getOutputSchema().getFieldCount();
                SortDefinition lhs_sort = lhs_clone->getSortOrder();

                for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].second;
                    rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
                }

                delete smj;

                Operator<B> *rhs_sorter = new Sort<B>(rhs_clone->clone(), rhs_sort);
                smj = new KeyedSortMergeJoin<B>(lhs_clone->clone(), rhs_sorter, kj->foreignKeyChild(),
                                                kj->getPredicate()->clone());
        } else if (smj->sortCompatible(rhs_clone) && !smj->sortCompatible(lhs_clone)) {
                auto join_key_idxs = smj->joinKeyIdxs();
                vector<ColumnSort> lhs_sort;
                SortDefinition rhs_sort = rhs_clone->getSortOrder();

                for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                    int idx = join_key_idxs[i].first;
                    lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
                }

                delete smj;

                Operator<B> *lhs_sorter = new Sort<B>(lhs_clone->clone(), lhs_sort);
                smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_clone->clone(), kj->foreignKeyChild(),
                                                kj->getPredicate()->clone());
                lhs_sorter->setParent(smj);
        } else if (! smj->sortCompatible(rhs_clone) && !smj->sortCompatible(lhs_clone)) {
            auto join_key_idxs = smj->joinKeyIdxs();
            vector<ColumnSort> lhs_sort;

            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].first;
                lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }

            // Sort the rhs according to the lhs sort order
            vector<ColumnSort> rhs_sort;
            int lhs_col_cnt = lhs_clone->getOutputSchema().getFieldCount();
            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].second;
                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
            }

            // Apply sorting to both lhs and rhs
            Operator<B> *lhs_sorter = new Sort<B>(lhs_clone->clone(), lhs_sort);
            Operator<B> *rhs_sorter = new Sort<B>(rhs_clone->clone(), rhs_sort);

            smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, kj->foreignKeyChild(),
                                            kj->getPredicate()->clone());
            lhs_sorter->setParent(smj);
            rhs_sorter->setParent(smj);
        }

        smj->setOperatorId(operatorId);
        smj->updateCollation();
        recurseNode(smj);

        delete smj;
        delete kj;

    } else if (join->getType() == OperatorType::KEYED_SORT_MERGE_JOIN) {
        // Create plan for SMJ first
        KeyedSortMergeJoin<B> *smj = (KeyedSortMergeJoin<B> *) join->clone();
        smj->setChild(lhs_clone, 0);
        smj->setChild(rhs_clone, 1);
        smj->updateCollation();

        int foreign_key = smj->foreignKeyChild();

        if (smj->sortCompatible(lhs_clone)) {
            auto join_key_idxs = smj->joinKeyIdxs();
            vector<ColumnSort> rhs_sort;
            int lhs_col_cnt = lhs_clone->getOutputSchema().getFieldCount();
            SortDefinition lhs_sort = lhs_clone->getSortOrder();

            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].second;
                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
            }

            if (!smj->sortCompatible(rhs_clone)) {
                Operator<B> *rhs_sorter = new Sort<B>(rhs_clone->clone(), rhs_sort);
                Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_clone->clone(), rhs_sorter, foreign_key,
                                                                 smj->getPredicate()->clone());
                new_smj->setOperatorId(operatorId);
                new_smj->updateCollation();
                recurseNode(new_smj);
                delete new_smj;
            } else {
                smj->getChild(1)->setSortOrder(rhs_sort);
                smj->updateCollation();
                recurseNode(smj);
            }
        } else if (smj->sortCompatible(rhs_clone)) {
            auto join_key_idxs = smj->joinKeyIdxs();
            vector<ColumnSort> lhs_sort;
            SortDefinition rhs_sort = rhs_clone->getSortOrder();

            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].first;
                lhs_sort.emplace_back(ColumnSort(idx, rhs_sort[i].second));
            }

            if (!smj->sortCompatible(lhs_clone)) {
                Operator<B> *lhs_sorter = new Sort<B>(lhs_clone->clone(), lhs_sort);
                Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_clone->clone(), foreign_key,
                                                                 smj->getPredicate()->clone());
                new_smj->setOperatorId(operatorId);
                new_smj->updateCollation();
                recurseNode(new_smj);
                delete new_smj;
            } else {
                smj->getChild()->setSortOrder(lhs_sort);
                smj->updateCollation();
                recurseNode(smj);
            }
        }
        else if (!smj->sortCompatible(rhs_clone) && !smj->sortCompatible(lhs_clone)) {
            auto join_key_idxs = smj->joinKeyIdxs();

            // Sort the lhs according to the rhs sort order
            vector<ColumnSort> lhs_sort;
            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].first;
                lhs_sort.emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            int lhs_col_cnt = lhs_clone->getOutputSchema().getFieldCount();

            // Sort the rhs according to the lhs sort order
            vector<ColumnSort> rhs_sort;
            for (size_t i = 0; i < join_key_idxs.size(); ++i) {
                int idx = join_key_idxs[i].second;
                rhs_sort.emplace_back(ColumnSort(idx - lhs_col_cnt, lhs_sort[i].second));
            }
            // Apply sorting to both lhs and rhs
            Operator<B> *lhs_sorter = new Sort<B>(lhs_clone->clone(), lhs_sort);
            Operator<B> *rhs_sorter = new Sort<B>(rhs_clone->clone(), rhs_sort);

            Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs_sorter, smj->foreignKeyChild(),
                                            smj->getPredicate()->clone());
            new_smj->setOperatorId(operatorId);
            lhs_sorter->setParent(new_smj);
            rhs_sorter->setParent(new_smj);
            recurseNode(new_smj);
            delete new_smj;
        }
        else {
            smj->updateCollation();
            recurseNode(smj);
        }

        // Create plan for NLJ next.
        KeyedJoin<B> *kj = new KeyedJoin(lhs_clone->clone(), rhs_clone->clone(), foreign_key,
                                         smj->getPredicate()->clone());
        kj->setOperatorId(operatorId);
        kj->updateCollation();

        recurseNode(kj);

        delete kj;
        delete smj;
    }
}

template<typename B>
void PlanOptimizer<B>::recurseAgg(Operator<B> *agg) {

    Operator<B> *child = agg->getChild();
    int child_id = child->getOperatorId();

    // If there is sort before agg, then id will be -1, so get real child's id
    if(child_id < 0)
        child = child->getChild();

    // Nested Loop Aggregate Case
    if (agg->getType() == OperatorType::NESTED_LOOP_AGGREGATE) {
        NestedLoopAggregate<B> *nla = (NestedLoopAggregate<B> *) agg;

        // agg is cloned from operators_ map in the optimizeHelper()
        // it's child is pointing operators_ map's child(original child), so need to fix those to point child with new order
        nla->updateCollation();
        recurseNode(nla);

        std::vector<int32_t> group_by_ordinals = nla->group_by_;
        SortDefinition child_sort = child->getSortOrder();
        SortDefinition effective_sort = nla->effective_sort_;

        // If Child's sort is not compatible with effective_sort, then insert sort before sma.
        if (child_sort != effective_sort) {
            Sort<B> *sort_before_sma = new Sort<B>(child->clone(), effective_sort);
            SortMergeAggregate<B> *sma = new SortMergeAggregate<B>(sort_before_sma, group_by_ordinals, nla->aggregate_definitions_, effective_sort, nla->getCardinalityBound());
            sma->setOperatorId(nla->getOperatorId());
            sma->updateCollation();
            recurseNode(sma);
        }
            // If Child's sort is compatible with effective_sort, then just insert sma.
        else {
            SortMergeAggregate<B> *sma = new SortMergeAggregate<B>(child->clone(), group_by_ordinals, nla->aggregate_definitions_, effective_sort, nla->getCardinalityBound());
            sma->setOperatorId(nla->getOperatorId());
            sma->updateCollation();
            recurseNode(sma);
        }
    }
        // SortMergeAggregate Case
    else {
        SortMergeAggregate<B> *sma = (SortMergeAggregate<B> *) agg;

        SortDefinition child_sort = child->getSortOrder();
        std::vector<int32_t> group_by_ordinals = sma->group_by_;
        SortDefinition effective_sort = sma->effective_sort_;

        // If Child's sort is not compatible with effective_sort, then insert sort before sma.
        if(child_sort != effective_sort){
            Sort<B> *sort_before_sma = new Sort<B>(child, effective_sort);
            sma->setChild(sort_before_sma);
        }
        // If Child's sort is compatible with effective_sort, then just insert sma.
        sma->updateCollation();
        recurseNode(sma);

        // Make plan in case of nla.
        NestedLoopAggregate<B> *nla = new NestedLoopAggregate(child->clone(), sma->group_by_, sma->aggregate_definitions_, sma->effective_sort_, sma->getCardinalityBound());
        nla->setOperatorId(sma->getOperatorId());
        nla->updateCollation();
        recurseNode(nla);
    }
}

template<typename B>
void PlanOptimizer<B>::recurseSort(Operator<B> *sort) {
    Operator<B> *child = sort->getChild();

    Sort<B> *s = (Sort<B> *) sort;

    // If sort does not have LIMIT clause, then need to check if this sort is needed
    if(s->getOutputCardinality() > 0) {
        // If sort is the same with child's order, then this sort is not needed
        if (sort->getSortOrder() == child->getSortOrder()) {
            int op_id = sort->getOperatorId();

            // If sort is not root, then need to fix parent's child pointer
            if (op_id < getRoot()->getOperatorId()) {
                Operator<B> *parent = operators_.at(sort->getParent()->getOperatorId());
                parent->setChild(child);
                recurseNode(parent);
            } else {   // If sort was root
                s->setChild(nullptr);
                child->setParent(nullptr);

                size_t plan_cost = child->planCost();
                total_plan_cnt_++;

                if(plan_cost < min_plan_cost_) {
                    min_plan_cost_ = plan_cost;
                    if(min_cost_plan_ != nullptr)
                        delete min_cost_plan_;
                    min_cost_plan_ = child->clone();
                }
                return;
            }
        }
            // If sort is needed
        else {
            recurseNode(sort);
        }
    }
    else{
        recurseNode(sort);
    }
}

template class vaultdb::PlanOptimizer<bool>;
template class vaultdb::PlanOptimizer<emp::Bit>;