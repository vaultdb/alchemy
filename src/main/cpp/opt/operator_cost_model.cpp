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
    // Cost : Sort_Cost + Input_Row_Count * Agg_Cost_Per_Row(=per_row_cost)

    size_t input_row_count = aggregate->getOutputCardinality();
    Expression<Bit> *predicate = aggregate->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    size_t sort_cost = sortCost(aggregate->getOutputSchema(), aggregate->getSortOrder(), input_row_count);

    ExpressionCostModel<Bit> cost_model(root, aggregate->getOutputSchema());
    size_t per_row_cost = cost_model.cost();

    return sort_cost + per_row_cost * input_row_count;
}

size_t OperatorCostModel::nestedLoopAggregateCost(const NestedLoopAggregate<Bit> *aggregate) {
    // Cost : Input_Row_Count * Output_Cardinality * Agg_Cost_Per_Row(=per_row_cost)

    Expression<Bit> *predicate = aggregate->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, aggregate->getOutputSchema());
    size_t per_row_cost = cost_model.cost();
    size_t input_row_count = aggregate->getChild()->getOutputCardinality();
    size_t output_cardinality = aggregate->getOutputCardinality();
    return per_row_cost * input_row_count * output_cardinality;
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
    QuerySchema input_schema = aggregate->getChild()->getOutputSchema();
    QuerySchema output_schema = aggregate->getOutputSchema();
    int agg_idx = 0;
    // ScalarAggregate : Without GroupBy Clause,
    // Cost : Input_Row_Count * Agg_Cost_Per_Row(=per_row_cost)
    auto agg_defs = aggregate->aggregate_definitions_;
    size_t per_row_cost = 0L;

    for(auto agg_def : agg_defs) {
        int ordinal = agg_def.ordinal;
        QueryFieldDesc f = input_schema.getField(ordinal);
        AggregateId agg_type = agg_def.type;

        /* condition_write in scalar_aggregate_impl.cpp :
         * to_accumulate = (!input_dummy) & (!not_initialized);
         * B to_initialize = (!input_dummy) & not_initialized;
        */
        size_t conditional_write;
        size_t initialize_cost;
        size_t aggregator_cost;

        cout << "ordinal : " << ordinal << ", Agg Idx : " << agg_idx << ", Agg type : " << agg_def.toString() << endl;
        switch(agg_type) {
            case AggregateId::MIN:
            case AggregateId::MAX: {
                conditional_write = f.size() + f.bitPacked();
                /* initialize_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_initialize, input_field, output_field);
                */
                initialize_cost = f.size() + f.bitPacked();

                /* compare_cost in scalar_aggregate_impl.cpp
                 * IN MIN
                 * accumulated = Field<B>::If(to_accumulate & (input_field < output_field), input_field, accumulated);
                 * In MAX
                 * accumulated = Field<B>::If(to_accumulate & (input_field > output_field), input_field, accumulated);
                */
                size_t compare_cost = ExpressionCostModel<Bit>::getComparisonCost(f) + 2;
                cout << ", Conditional Write : " << conditional_write << ", Initialize Cost : " << initialize_cost << ", Compare Cost : " << compare_cost ;
                aggregator_cost = conditional_write + initialize_cost + compare_cost;
                cout << ", Aggregator Cost : " << aggregator_cost << endl;
                break;
            }
            case AggregateId::COUNT: {
                cout << "Output Schema : " << output_schema << endl;
                QueryFieldDesc f_output = output_schema.getField(agg_idx);
                conditional_write = f_output.size() + f_output.bitPacked();

                /* initialize_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_initialize, one, output_field);
                */
                initialize_cost = f_output.size() + f_output.bitPacked();
                cout << "Output Size : " << f_output.size() << ", Bit Packed : " << f_output.bitPacked();

                /* add_subtraction_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_accumulate, accumulated + one, accumulated);
                */
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                cout << ", Condtional Write : " << conditional_write << ", initialize cost : " << initialize_cost << ", add_subtraction_cost : " << add_subtraction_cost;
                aggregator_cost = conditional_write + initialize_cost + add_subtraction_cost;
                cout << ", Aggregator Cost : " << aggregator_cost << endl;
                break;
            }
            case AggregateId::SUM: {
                cout << "Output Schema : " << output_schema << endl;
                QueryFieldDesc f_output = output_schema.getField(agg_idx);
                conditional_write = f_output.size() + f_output.bitPacked();

                /* initialize_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_initialize, input_field, output_field);
                */
                initialize_cost = 2 * (f_output.size() + f_output.bitPacked());
                cout << "Output Size : " << f_output.size() << ", Bit Packed : " << f_output.bitPacked();

                /* add_subtraction_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_accumulate, accumulated + input_field, accumulated);
                */
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                cout << ", Condtional Write : " << conditional_write << ", initialize cost : " << initialize_cost << ", add_subtraction_cost : " << add_subtraction_cost;
                aggregator_cost = conditional_write + initialize_cost + add_subtraction_cost;
                cout << ", Aggregator Cost : " << aggregator_cost << endl;
                break;
            }
            case AggregateId::AVG: {
                cout << "Output Schema : " << output_schema << endl;
                QueryFieldDesc f_output = output_schema.getField(agg_idx);

                cout << "Output Size : " << f_output.size() << ", Bit Packed : " << f_output.bitPacked() << " AddSubtraction : " << ExpressionCostModel<Bit>::getAddSubtractionCost(f_output) << endl;
                conditional_write = f_output.size() + f_output.bitPacked();
                size_t count_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t sum_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                cout << "Count Cost : " << count_cost << ", Sum Cost : " << sum_cost;
                size_t divide_cost = 1212;
                cout << ", Condtional Write : " << conditional_write << ", count_cost : " << count_cost << ", sum_cost : " << sum_cost;
                aggregator_cost = conditional_write + count_cost + sum_cost + divide_cost;
                cout << ", Aggregator Cost : " << aggregator_cost << endl;
                break;
            }
            default:
                aggregator_cost = 0;

        }
        // not_initialized = (to_initialize & !not_initialized) | (to_accumulate & not_initialized);
        aggregator_cost += 4;

        per_row_cost += aggregator_cost;

        cout << "After " << agg_def.toString() << " : " << per_row_cost << "\n" << endl;
        ++agg_idx;
    }

    size_t row_count = aggregate->getChild()->getOutputCardinality();
    return per_row_cost * row_count;
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

