#include "opt/operator_cost_model.h"
#include "opt/expression_cost_model.h"
#include <expression/generic_expression.h>


using namespace vaultdb;

size_t OperatorCostModel::operatorCost(const SecureOperator *op) {
    string operator_type = op->getOperatorType();
    if(operator_type == "Filter")
        return filterCost((Filter<Bit> *) op);
    else if(operator_type == "SecureSqlInput")
        return secureSqlInputCost((SecureSqlInput *) op);
    else
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

    throw std::invalid_argument("SecureSqlInput bitonic merge not yet implemented!");
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
    return 0;
}

size_t OperatorCostModel::sortMergeJoinCost(const SortMergeJoin<Bit> *join) {
    return 0;
}

size_t OperatorCostModel::groupByAggregateCost(const GroupByAggregate<Bit> *aggregate) {
    return 0;
}

size_t OperatorCostModel::nestedLoopAggregateCost(const NestedLoopAggregate<Bit> *aggregate) {
    return 0;
}

size_t OperatorCostModel::sortCost(const Sort<Bit> *sort) {
    return 0;
}

size_t OperatorCostModel::shrinkwrapCost(const Shrinkwrap<Bit> *shrinkwrap) {
    return 0;
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
    return 0;
}

size_t OperatorCostModel::unionCost(const Union<Bit> *union_op) {
    return 0;
}

size_t OperatorCostModel::zkSqlInputCost(const ZkSqlInput<Bit> *input) {
    return 0;
}


