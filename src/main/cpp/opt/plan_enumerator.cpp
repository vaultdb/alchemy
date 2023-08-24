#include <opt/plan_enumerator.h>
#include <util/data_utilities.h>
#include <operators/keyed_join.h>
#include <operators/sort_merge_join.h>
#include <operators/sort_merge_aggregate.h>
#include <operators/nested_loop_aggregate.h>
#include <operators/sort.h>
#include <util/logger.h>

using namespace Logging;
using namespace vaultdb;

PlanEnumerator::PlanEnumerator(SecureOperator *root, const map<int, vector<SortDefinition >> &sort_orders) {
    interesting_sort_orders_ = sort_orders;
    root_ = root;
    setup(root_);
}



void PlanEnumerator::setup(SecureOperator *op) {
        tree_nodes_[op->getOperatorId()] = op;

        if(op->getChild() != nullptr) {
            setup(op->getChild());
        }
        if(op->getChild(1) != nullptr) {
            setup(op->getChild(1));
        }

}

// optimize a left-deep tree with a fixed join order
// optimize operator implementations and sort placement

// Plan enumerator to iteratively try all potential impls for each operator declared in query plan.
//  * For leafs/LogicalValues cycle through all interesting sort orders then recurse to next tree layer
//  * For joins cycle through NLJ vs SMJ (assume no partial sort opt for now)
//  * For aggs try NLA and SMA (if a card bound exists)
//  * For SMA check if we need sort before op.
//  * After SMA assess if we still need any sort that's a parent to it.

SecureOperator *PlanEnumerator::optimizeTree() {
    // find left-most leaf
    SecureOperator *leaf = root_;
    while(leaf->getChild() != nullptr) {
        leaf = leaf->getChild();
    }

    vector<SortDefinition> sorts = getCollations(leaf);
    assert(leaf->getType() == OperatorType::SECURE_SQL_INPUT);

    for(auto sort : sorts) {
        auto input = leaf->clone();
        if(input->getSortOrder() != sort)  input->setSortOrder(sort); // automatically sorts leaf inputs by new sort order
        optimizeTreeHelper(input);
    }
    return min_cost_plan_;

}

void PlanEnumerator::optimizeTreeHelper(SecureOperator *op) {
    // try out new parents for op
    SecureOperator *parent = tree_nodes_[op->getOperatorId()]->getParent();
    SecureOperator *node = parent->clone();
    node->setChild(op);

    switch(node->getType()) {
        case OperatorType::SQL_INPUT:
        case OperatorType::SECURE_SQL_INPUT:
        case OperatorType::ZK_SQL_INPUT:
        case OperatorType::TABLE_INPUT: {
            // first try with given sort order (if any) and empty set unconditionally
            // it is unlikely this code will be run since join covers this instead
            auto sorts = getCollations(op);
            for (auto &sort: sorts) {
                node->setSortOrder(sort);
                 recurseNode(node);
            }
            break;
        }
        case OperatorType::KEYED_NESTED_LOOP_JOIN:
        case OperatorType::SORT_MERGE_JOIN: {
            // for each rhs leaf collation cycle through its collations and then try both SMJ and NLJ with each collation
             recurseJoin(node);
             delete node;
             break;
        }
        // skip sorts for now, we handle them elsewhere
//        case OperatorType::SORT: {
//            auto sort_parent = tree_nodes_[node->getOperatorId()]->getParent();
//            if(sort_parent != nullptr) {
//                // leapfrog over sort
//                auto next_node = sort_parent->clone();
//                next_node->setChild(op->clone());
//                recurseNode(next_node);
//            }
//            else {
//               // no parent, just recurse to check e2e cost
//            recurseNode(node);
//            }

//
//        }

        // just working on joins and sql input for now, all others just clone and recurse
//        case OperatorType::NESTED_LOOP_AGGREGATE:
//        case OperatorType::SORT_MERGE_AGGREGATE: {
//            recurseAgg(node);
//            break;
//        }
        default:
            recurseNode(node);
            break;
    }



}

void PlanEnumerator::recurseJoin(SecureOperator *join) {
   // for now assume that child is always SecureSqlInput
    assert(join->getChild(1)->getType() == OperatorType::SECURE_SQL_INPUT);

    auto rhs_sorts = getCollations(join->getChild(1));
    for(auto &sort: rhs_sorts) {
        auto rhs_leaf = join->getChild(1)->clone();
        rhs_leaf->setSortOrder(sort);
        auto first_join = join->clone();
        first_join->setChild(rhs_leaf, 1);
        recurseNode(first_join->clone());

        if(join->getType() == OperatorType::KEYED_NESTED_LOOP_JOIN) {
            KeyedJoin<Bit> *kj = (KeyedJoin<Bit> *) first_join;
            auto second_join  = new SortMergeJoin<Bit>(kj->getChild(0)->clone(), kj->getChild(1)->clone(), kj->foreignKeyChild(), kj->getPredicate());
            second_join->setOperatorId(kj->getOperatorId());
            recurseNode(second_join->clone());
            delete second_join;
        }
        else {
            SortMergeJoin<Bit> *smj = (SortMergeJoin<Bit> *) first_join;
            auto second_join = new KeyedJoin<Bit>(smj->getChild(0)->clone(), smj->getChild(1)->clone(), smj->foreignKeyChild(), smj->getPredicate());
            second_join->setOperatorId(smj->getOperatorId());
            recurseNode(second_join->clone());
            delete second_join;
        }
       // delete first_join;
    }
}

void PlanEnumerator::recurseAgg(SecureOperator *agg) {
    SecureOperator *child = agg->getChild();

    if (agg->getType() == OperatorType::NESTED_LOOP_AGGREGATE) {
        NestedLoopAggregate<Bit> *nla = (NestedLoopAggregate<Bit> *) agg;
        std::vector<int32_t> group_by_ordinals = nla->group_by_;
        // if sort not aligned, insert a sort op
        SortDefinition child_sort = child->getSortOrder();
        if (!nla->sortCompatible(child_sort)) {
            // insert sort
            SortDefinition child_sort;
            for (uint32_t idx: group_by_ordinals) {
                child_sort.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            Sort<Bit> *sort_before_sma = new Sort<Bit>(child->clone(), child_sort);
            SortMergeAggregate a(sort_before_sma->clone(), group_by_ordinals, nla->aggregate_definitions_, nla->effective_sort_);
            child->setParent(sort_before_sma);
            sort_before_sma->setChild(child);
            sort_before_sma->setParent(&a);
            a.setChild(sort_before_sma);
            recurseNode(&a);
        } else {
            SortMergeAggregate a(child->clone(), group_by_ordinals, nla->aggregate_definitions_, nla->effective_sort_, nla->getCardinalityBound());
            recurseNode(&a);
        }
        recurseNode(nla);
    } else {
        SortMergeAggregate<Bit> *sma = (SortMergeAggregate<Bit> *) agg;

        SortDefinition cur_sort = sma->getSortOrder();
        std::vector<int32_t> group_by_ordinals = sma->group_by_;
        if (!((GroupByAggregate<Bit> *) agg)->sortCompatible(cur_sort)) {
            // insert sort
            SortDefinition sort_order;
            for (uint32_t idx: group_by_ordinals) {
                sort_order.template emplace_back(ColumnSort(idx, SortDirection::ASCENDING));
            }
            Sort<Bit> *sort_before_sma = new Sort<Bit>(child->clone(), sort_order);
            child->setParent(sort_before_sma);
            sort_before_sma->setChild(child);
            sort_before_sma->setParent(sma);
            sma->setChild(sort_before_sma);
        }


        recurseNode(sma);
        NestedLoopAggregate a(child->clone(), sma->group_by_, sma->aggregate_definitions_, sma->effective_sort_,
                              sma->getCardinalityBound());

        // In NestedLoop, child's sort is not remained,
        // TODO : need to change this part of setting sort order.
        a.setSortOrder(child->getSortOrder());
        a.setOperatorId(sma->getOperatorId());

        //TODO : check if we need to add sort order to parent. Need Expected Sort

        recurseNode(&a);
    }
}

void PlanEnumerator::recurseNode(SecureOperator *op) {
    int op_id = op->getOperatorId();
    SecureOperator *parent = tree_nodes_[op_id]->getParent();

    // at the root node
    if(parent == nullptr) {
        size_t plan_cost = op->planCost();
        ++plan_cnt_;

        Logger* logger = get_log();
        stringstream ss;

        ss << "Reached root! " << endl;
        ss << op->printTree() << endl;
        ss << "Cost : " << std::to_string(plan_cost) << "\n" << "----------------------------" << endl;
        logger->write(ss.str(), Level::ALL);

        if(plan_cost < min_cost_) {
            min_cost_ = plan_cost;
            if(min_cost_plan_ != nullptr) {
                delete min_cost_plan_;
            }
            min_cost_plan_ = op;
        }
        else {
            delete op;
        }
        return;
    }

    // else recurse up the tree
    optimizeTreeHelper(op);

}

