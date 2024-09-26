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
            if(secureSqlInputOp->getInputParty() > 0) {
                recurseNode(node); } else {
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

        // parent is pointing orginal child with original sort order
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

    auto rhs_leaf = fetchLeaf(join->getChild(1));
    auto rhs_sorts = getCollations(rhs_leaf);
    Operator<B> *meta_lhs = join->getChild();

    for(auto &sort: rhs_sorts) {
        // Need to clone lhs and rhs, because after every plan, that plan will be deleted.
        Operator<B> *lhs = meta_lhs->clone();
        Operator<B> *rhs = rhs_leaf->clone();
        rhs->setSortOrder(sort);

        if(join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {
            // For KeyedJoin, sort order is same with foreign key's sort order
            KeyedJoin<B> *kj = (KeyedJoin<B> *)join;

            // Need to reset right child as rhs because kj's rhs was pointing original child, which does not applied new sort order
            kj->setChild(rhs, 1);
            kj->updateCollation();

            recurseNode(kj);

            // For SMJ, sort order is same with sort key's order
            // Make plan in case of smj.
            KeyedSortMergeJoin<B> *smj = new KeyedSortMergeJoin(lhs->clone(), rhs->clone(), kj->foreignKeyChild(), kj->getPredicate()->clone());

            // check sort compatibility for SMJ
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
            } else if (smj->sortCompatible(rhs)) {
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

            smj->setOperatorId(kj->getOperatorId());
            smj->updateCollation();
            recurseNode(smj);

            //Since smj was not original node, delete it
            delete smj;
        } else if(join->getType() == OperatorType::KEYED_SORT_MERGE_JOIN){
            KeyedSortMergeJoin<B> *smj = (KeyedSortMergeJoin<B> *) join;

            // Need to reset right child as rhs because kj's rhs was pointing original child, which does not applied new sort order
            smj->setChild(rhs, 1);
            smj->updateCollation();

//            Expression<B> *predicate;
            int foreign_key = smj->foreignKeyChild();
            int operatorId = smj->getOperatorId();
//            bool new_smj_flag = false;

            // check sort compatibility for SMJ

            // If lhs is sort compatible, then add sort at rhs.
            if (smj->sortCompatible(lhs)) {
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

                // If rhs is not scan, then add sort operator
                // If rhs is scan, then do not add sort operator, just setSortOrder and updateCollation
                if(rhs->getType() != OperatorType::SECURE_SQL_INPUT) {
                    // Define sort before rhs_leaf
                    Operator<B> *rhs_sorter = new Sort<B>(rhs->clone(), rhs_sort);
                    Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs->clone(), rhs_sorter, foreign_key, smj->getPredicate()->clone());
                    new_smj->setOperatorId(operatorId);
                    new_smj->updateCollation();
                    recurseNode(new_smj);
                    delete new_smj;
                } else {  //If rhs is scan, set scan's sort order and update collation
                    smj->getChild(1)->setSortOrder(rhs_sort);
                    smj->updateCollation();
                    recurseNode(smj);
                }
            } else if (smj->sortCompatible(rhs)) {   // If rhs is sort compatible, then add sort at lhs.
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

                // If lhs is not scan, then add sort operator
                // If lhs is scan, then do not add sort operator, just setSortOrder and updateCollation
                if(lhs->getType() != OperatorType::SECURE_SQL_INPUT) {
                    // Define sort before lhs
                    Operator<B> *lhs_sorter = new Sort<B>(lhs->clone(), lhs_sort);
                    Operator<B> *new_smj = new KeyedSortMergeJoin<B>(lhs_sorter, rhs->clone(), foreign_key, smj->getPredicate()->clone());
                    new_smj->setOperatorId(operatorId);
                    new_smj->updateCollation();
                    recurseNode(new_smj);
                    delete new_smj;
                } else {    // If lhs is scan, set scan's sort order and update collation
                    smj->getChild()->setSortOrder(lhs_sort);
                    smj->updateCollation();
                    recurseNode(smj);
                }
            } else {   // If both side is not sort compatible, then just do sort merge join
                smj->updateCollation();
                recurseNode(smj);
            }

            // Make plan in case of KeyedJoin.
            KeyedJoin<B> *kj = new KeyedJoin(lhs->clone(), rhs->clone(), foreign_key, smj->getPredicate()->clone());
            kj->setOperatorId(smj->getOperatorId());
            kj->updateCollation();

            recurseNode(kj);

            //Since kj was not original node, delete it
            delete kj;
        }
    }
}

template<typename B>
void PlanOptimizer<B>::recurseAgg(Operator<B> *agg) {

    Operator<B> *child = agg->getChild();
    int child_id = child->getOperatorId();

    // If there is sort before agg, then id will be -1, so get real child's id
    if(child_id < 0)
        child = agg->getChild();

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
        } else {  // If Child's sort is compatible with effective_sort, then just insert sma.
            SortMergeAggregate<B> *sma = new SortMergeAggregate<B>(child->clone(), group_by_ordinals, nla->aggregate_definitions_, effective_sort, nla->getCardinalityBound());
            sma->setOperatorId(nla->getOperatorId());
            sma->updateCollation();
            recurseNode(sma);
        }
    } else {  // SortMergeAggregate Case
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
        } else {  // If sort is needed
            recurseNode(sort);
        }
    } else{
        recurseNode(sort);
    }
}

template class vaultdb::PlanOptimizer<bool>;
template class vaultdb::PlanOptimizer<emp::Bit>;