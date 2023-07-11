#include "opt/operator_cost_model.h"
#include "opt/expression_cost_model.h"
#include "query_table/row_table.h"
#include <expression/generic_expression.h>


using namespace vaultdb;

size_t OperatorCostModel::operatorCost(const SecureOperator *op) {
    string operator_type = op->getOperatorType();
    if(operator_type == "Filter")
        return filterCost((Filter<Bit> *) op);
    else if(operator_type == "SecureSqlInput")
        return secureSqlInputCost((SecureSqlInput *) op);
    else if(operator_type == "KeyedJoin")
        return keyedJoinCost((KeyedJoin<Bit> *) op);
    else if(operator_type == "BasicJoin")
        return basicJoinCost((BasicJoin<Bit> *) op);
    else if(operator_type == "Shrinkwrap")
        return shrinkwrapCost((Shrinkwrap<Bit> *) op);
    else if(operator_type == "Project")
        return projectCost((Project<Bit> *) op);
    else if(operator_type == "SortMergeJoin")
        return sortMergeJoinCost((SortMergeJoin<Bit> *) op);
    else if(operator_type == "GroupByAggregate")
        return groupByAggregateCost((GroupByAggregate<Bit> *) op);
    else if(operator_type == "NestedLoopAggregate")
        return nestedLoopAggregateCost((NestedLoopAggregate<Bit> *) op);
    else if(operator_type == "ScalarAggregate")
        return scalarAggregateCost((ScalarAggregate<Bit> *) op);

    return 0;
}
size_t vaultdb::OperatorCostModel::filterCost(const vaultdb::Filter<Bit> *filter) {
    Expression<Bit> *predicate = filter->predicate_;

    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, filter->getOutputSchema());
    size_t per_row_cost = cost_model.cost();
    size_t row_count = filter->getOutputCardinality();
    return per_row_cost * row_count;

}

size_t vaultdb::OperatorCostModel::secureSqlInputCost(const SecureSqlInput *input) {
    if(input->getSortOrder().empty())
        return 0;

    float n = input->getOutputCardinality();
    float comparisons = n * log2(n);
    size_t c_and_s_cost = compareSwapCost(input->getOutputSchema(), input->getSortOrder(), n);
    return c_and_s_cost * (size_t) comparisons;
    return comparisons * c_and_s_cost;
}

size_t OperatorCostModel::basicJoinCost(const BasicJoin<Bit> *join) {
    Expression<Bit> *predicate = join->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, join->getOutputSchema());
    size_t per_row_cost = cost_model.cost();
    size_t row_count = join->getOutputCardinality();
    return per_row_cost * row_count;

}

size_t OperatorCostModel::keyedJoinCost(const KeyedJoin<Bit> *join) {
    Expression<Bit> *predicate = join->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    SecureOperator *lhs = join->getChild(0);
    SecureOperator *rhs = join->getChild(1);

    size_t swap_cost = (join->foreignKeyChild() == 0) ? lhs->getOutputSchema().size() : rhs->getOutputSchema().size();
    swap_cost -= 1; // don't swap dummy tag - this is handled separately

    size_t predicate_cost = ExpressionCostModel<Bit>(root, join->getOutputSchema()).cost();
    size_t tuple_comparison_cnt = lhs->getOutputCardinality() *  rhs->getOutputCardinality();

    size_t tuple_comparison_cost = predicate_cost + swap_cost + 4; // 4 for bookkeeping on dummy tag
    return tuple_comparison_cnt * tuple_comparison_cost;

}

size_t OperatorCostModel::sortMergeJoinCost(const SortMergeJoin<Bit> *join) {
    return 0;
}

// only count GroupByAggregate cost, sort cost is accounted for in Sort operator
// Sort is sometimes pushed out of MPC, thus separate accounting
size_t OperatorCostModel::groupByAggregateCost(const GroupByAggregate<Bit> *aggregate) {
    size_t n = aggregate->getOutputCardinality();

    throw;
}

size_t OperatorCostModel::nestedLoopAggregateCost(const NestedLoopAggregate<Bit> *aggregate) {
    return 0;
}

size_t OperatorCostModel::sortCost(const Sort<Bit> *sort) {
    size_t n = sort->getOutputCardinality();
    QuerySchema table_schema = sort->getOutputSchema();
    auto sort_def = sort->getSortOrder();
    return sortCost(table_schema, sort_def, n);

}

size_t OperatorCostModel::shrinkwrapCost(const Shrinkwrap<Bit> *shrinkwrap) {
    auto child = shrinkwrap->getChild(0);
    if(child->getOutputCardinality() < shrinkwrap->getOutputCardinality()) {
        return 0;
    }

    // re-derive sort cost, this is a shortcut to avoid creating a dummy table for sort with correct N
    return sortCost(shrinkwrap->getOutputSchema(), shrinkwrap->getSortOrder(), shrinkwrap->getOutputCardinality());

}

size_t OperatorCostModel::projectCost(const Project<Bit> *project) {
    auto exprs = project->getExpressions();
    auto exec_exprs = project->getExpressionsToExec();
    size_t row_cost = 0L;
    QuerySchema input_schema = project->getChild(0)->getOutputSchema();
    for(auto pos : exec_exprs) {
        Expression<Bit> *expression = exprs[pos];
        assert(expression->exprClass() == ExpressionClass::GENERIC);
        ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) expression)->root_;
        ExpressionCostModel<Bit> cost_model(root, input_schema);
        row_cost += cost_model.cost();
    }

    size_t row_count = project->getOutputCardinality();
    return row_cost * row_count;
}

size_t OperatorCostModel::scalarAggregateCost(const ScalarAggregate<Bit> *aggregate) {
     throw; // Not yet implemented
}

size_t OperatorCostModel::compareSwapCost(const QuerySchema &schema, const SortDefinition &sort, const int &tuple_cnt) {
    float comparison_cost = 0;

    for(auto col_sort : sort) {
        QueryFieldDesc f = schema.getField(col_sort.first);
        comparison_cost += 2 * ExpressionCostModel<Bit>::getComparisonCost(f);
        comparison_cost += 2; // bookkeeping overhead.  Bit::select (+1), & (+1)
    }

    size_t swap_cost = schema.size();
    return comparison_cost + swap_cost;
}



size_t OperatorCostModel::sortCost(const QuerySchema &schema, const SortDefinition &sort, const int &n) {
    float rounds = log2(n);
    float stages = (rounds * (rounds + 1))/2.0;

    float comparisons_per_stage = n / 2.0;
    float comparison_cnt = stages * comparisons_per_stage;


    // each comparison consists of an equality, a <, and a handful of ANDs
    size_t c_and_s_cost = compareSwapCost(schema, sort, n);
    return comparison_cnt * c_and_s_cost;
}

