#include "opt/operator_cost_model.h"
#include "opt/expression_cost_model.h"
#include "query_table/query_table.h"
#include <expression/generic_expression.h>
#include "expression/visitor/join_equality_condition_visitor.h"

using namespace vaultdb;

size_t OperatorCostModel::operatorCost(const SecureOperator *op) {
   switch(op->getType()) {
       case OperatorType::FILTER:
           return filterCost((Filter<Bit> *) op);
           case OperatorType::SECURE_SQL_INPUT:
               return secureSqlInputCost((SecureSqlInput *) op);
          case OperatorType::MERGE_INPUT:
              return mergeInputCost((MergeInput *) op);
            case OperatorType::SORT:
                return sortCost((Sort<Bit> *) op);
            case OperatorType::KEYED_NESTED_LOOP_JOIN:
                return keyedJoinCost((KeyedJoin<Bit> *) op);
            case OperatorType::NESTED_LOOP_JOIN:
                return basicJoinCost((BasicJoin<Bit> *) op);
            case OperatorType::MERGE_JOIN:
                return mergeJoinCost((MergeJoin<Bit> *) op);
           case OperatorType::KEYED_SORT_MERGE_JOIN:
              return keyedSortMergeJoinCost((KeyedSortMergeJoin<Bit> *) op);
           case OperatorType::SHRINKWRAP:
              return shrinkwrapCost((Shrinkwrap<Bit> *) op);
           case OperatorType::PROJECT:
                return projectCost((Project<Bit> *) op);
           case OperatorType::SORT_MERGE_AGGREGATE:
                return groupByAggregateCost((SortMergeAggregate<Bit> *) op);
           case OperatorType::NESTED_LOOP_AGGREGATE:
              return nestedLoopAggregateCost((NestedLoopAggregate<Bit> *) op);
           case OperatorType::SCALAR_AGGREGATE:
                return scalarAggregateCost((ScalarAggregate<Bit> *) op);
       default:
           return 0;
   }
}
size_t vaultdb::OperatorCostModel::filterCost(const vaultdb::Filter<Bit> *filter) {
    Expression<Bit> *predicate = filter->predicate_;

    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, filter->getOutputSchema());
    size_t per_row_cost = cost_model.cost() + 1; // +1 \dummy tag eval
    size_t row_count = filter->getOutputCardinality();
    return per_row_cost * row_count;

}

size_t vaultdb::OperatorCostModel::secureSqlInputCost(const SecureSqlInput *input) {
    if(input->getSortOrder().empty())
        return 0;

    if(input->plain_input_ != nullptr) {
        int local_input_card = input->plain_input_->tuple_cnt_;
        if(local_input_card == 0 || local_input_card == input->getOutputCardinality()) {
            return 0;
        }
    }

    float n =  input->getOutputCardinality();
    float rounds = log2(n);

    float comparisons_per_stage = n /2;
    float comparison_cnt = rounds * comparisons_per_stage;

    auto schema = input->getOutputSchema();
    auto sort = input->getSortOrder();

    size_t c_and_s_cost = compareSwapCost(schema, sort, n);
    return c_and_s_cost * (size_t) comparison_cnt;

}

size_t OperatorCostModel::basicJoinCost(const BasicJoin<Bit> *join) {
    Expression<Bit> *predicate = join->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, join->getOutputSchema());
    size_t per_row_cost = cost_model.cost() + 3; // +3 for bookkeeping on dummy tag
    size_t row_count = join->getOutputCardinality();
    return per_row_cost * row_count;

}

size_t OperatorCostModel::mergeJoinCost(MergeJoin<Bit> *join) {
    Expression<Bit> *predicate = join->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;

    ExpressionCostModel<Bit> cost_model(root, join->getOutputSchema());
    size_t per_row_cost = cost_model.cost() + 3; // +3 for bookkeeping on dummy tag
    size_t row_count = join->getOutputCardinality();
    return per_row_cost * row_count;
}


size_t OperatorCostModel::keyedJoinCost(const KeyedJoin<Bit> *join) {
    Expression<Bit> *predicate = join->getPredicate();
    assert(predicate->exprClass() == ExpressionClass::GENERIC);
    ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) predicate)->root_;
    size_t predicate_cost = ExpressionCostModel<Bit>(root, join->getOutputSchema()).cost();

    SecureOperator *lhs = join->getChild(0);
    SecureOperator *rhs = join->getChild(1);

    size_t swap_cost = (join->foreignKeyChild() == 0) ? rhs->getOutputSchema().size() : lhs->getOutputSchema().size();
    swap_cost -= 1; // don't swap dummy tag - this is handled separately

    size_t tuple_comparison_cnt = lhs->getOutputCardinality() *  rhs->getOutputCardinality();
    size_t tuple_comparison_cost = predicate_cost + swap_cost + 3; // 3 for bookkeeping on dummy tag
    return tuple_comparison_cnt * tuple_comparison_cost;
}

size_t OperatorCostModel::keyedSortMergeJoinCost(KeyedSortMergeJoin<Bit> *join) {
	size_t cost = 0;

	Operator<Bit>* lhs = join->getChild(0);
	Operator<Bit>* rhs = join->getChild(1);
	QuerySchema augmented_schema = join->deriveAugmentedSchema();

	//first we have the cost of two sorts from augmentTables
	auto p = (GenericExpression<Bit>*) join->getPredicate();
	JoinEqualityConditionVisitor<Bit> join_visitor(p->root_);
	vector<pair<uint32_t, uint32_t> > join_idxs  = join_visitor.getEqualities();
	SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join_idxs.size());

	size_t table_id_idx = augmented_schema.getFieldCount() - 1;
	sort_def.emplace_back(table_id_idx, SortDirection::ASCENDING);

	size_t augment_cost = 0;
	if(join->sortCompatible()) {
		float n =  lhs->getOutputCardinality() + rhs->getOutputCardinality();
		float rounds = log2(n);

		float comparisons_per_stage = n /2;
		float comparison_cnt = rounds * comparisons_per_stage;

		size_t c_and_s_cost = compareSwapCost(augmented_schema, sort_def, n);
		augment_cost += comparison_cnt * c_and_s_cost;
	}
	else {
		augment_cost += sortCost(augmented_schema, sort_def, lhs->getOutputCardinality() + rhs->getOutputCardinality());
	}

	sort_def.clear();
    sort_def.emplace_back(table_id_idx, SortDirection::ASCENDING);
    for(int i = 0; i < join_idxs.size(); ++i) {
        sort_def.emplace_back(i, SortDirection::ASCENDING);
    }

	augment_cost += sortCost(augmented_schema, sort_def, lhs->getOutputCardinality() + rhs->getOutputCardinality());
	//std::cout << "Predicted cost of augmentTables: " << augment_cost << "\n";

	cost += augment_cost;

	//next we have the cost of the sort from obliviousDistribute
	QueryFieldDesc weight(augmented_schema.getFieldCount(), "weight", "", FieldType::SECURE_INT);
    augmented_schema.putField(weight);
    QueryFieldDesc is_new(augmented_schema.getFieldCount(), "is_new", "", FieldType::SECURE_INT);
    augmented_schema.putField(is_new);
    augmented_schema.initializeFieldOffsets();

	size_t is_new_idx = augmented_schema.getFieldCount() - 1;
    size_t weight_idx = augmented_schema.getFieldCount() - 2;

	SortDefinition second_sort_def{ ColumnSort(is_new_idx, SortDirection::ASCENDING), ColumnSort(weight_idx, SortDirection::ASCENDING)};

	size_t distribute_cost = 0;

	distribute_cost += sortCost(augmented_schema, second_sort_def, (join->foreignKeyChild() == 0) ? rhs->getOutputCardinality() : lhs->getOutputCardinality());

	//std::cout << "Predicted cost of sorting in obliviousDistribute: " << distribute_cost << "\n";

	//obliviousDistribute
	size_t n = join->getOutputCardinality();
	float inner_loop = n;
	float outer_loop = floor(log2(inner_loop));
	 
	float comparisons = outer_loop * inner_loop;
    size_t c_and_s_cost = compareSwapCost(augmented_schema, second_sort_def, n);
	distribute_cost += c_and_s_cost * (size_t) comparisons;

	//std::cout << "Predicted cost of obliviousDistribute (distribute step only): " << distribute_cost << "\n";

    cost += 2 * distribute_cost;
	
	//cost of conditional write step from expand
	InputReference<Bit> read_field(is_new_idx, augmented_schema);
	Field<Bit> one(FieldType::SECURE_INT, emp::Integer(32, 1));
	LiteralNode<Bit> constant_input(one);
	EqualNode equality_check((ExpressionNode<Bit> *) &read_field, (ExpressionNode<Bit> *) &constant_input);

	ExpressionCostModel<Bit> model(&equality_check, augmented_schema);
	auto if_cost = model.cost();	

	size_t conditional_write_cost = n * (if_cost + augmented_schema.size() + 2);	

	//std::cout << "Predicted cost of conditional write step: " << conditional_write_cost << "\n";

	cost += 2 * conditional_write_cost;
	
    return cost;
}

// only count SortMergeAggregate cost, sort cost is accounted for in Sort operator
// Sort is sometimes pushed out of MPC, thus separate accounting
size_t OperatorCostModel::groupByAggregateCost(const SortMergeAggregate<Bit> *aggregate) {
    // Cost : Sort_Cost + Input_Row_Count * Agg_Cost_Per_Row(=per_row_cost)
    QuerySchema input_schema = aggregate->getChild()->getOutputSchema();
    QuerySchema output_schema = aggregate->getOutputSchema();

    size_t input_row_count = aggregate->getChild()->getOutputCardinality();

    auto group_by_defs = aggregate->group_by_;
    size_t group_by_cost = 0L;
    for(auto group_by : group_by_defs) {
        QueryFieldDesc f = input_schema.getField(group_by);
        group_by_cost += 2 *( f.size() + f.bitPacked());
    }
    group_by_cost += 4;

    int output_ordinal = group_by_defs.size();
    // ScalarAggregate : Without GroupBy Clause,
    // Cost : Input_Row_Count * Agg_Cost_Per_Row(=per_row_cost)
    auto agg_defs = aggregate->aggregate_definitions_;
    size_t per_row_cost = 0L;

    for(auto agg_def : agg_defs) {
        int ordinal = agg_def.ordinal;
        QueryFieldDesc f = input_schema.getField(ordinal);
        AggregateId agg_type = agg_def.type;

        size_t conditional_write;
        size_t initialize_cost;
        size_t aggregator_cost;

        switch(agg_type) {
            case AggregateId::MIN:
            case AggregateId::MAX: {
                conditional_write = f.size() + f.bitPacked();

                initialize_cost = f.size() + f.bitPacked();

                size_t compare_cost = ExpressionCostModel<Bit>::getComparisonCost(f) + 2;
                aggregator_cost = group_by_cost + conditional_write + initialize_cost + compare_cost + 4;
                break;
            }
            case AggregateId::COUNT: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);
                conditional_write = f_output.size() + f_output.bitPacked();

                initialize_cost = f_output.size() + f_output.bitPacked();

                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = group_by_cost + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::SUM: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);
                conditional_write = f_output.size() + f_output.bitPacked();

                initialize_cost = 2 * (f_output.size() + f_output.bitPacked());
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = group_by_cost + conditional_write + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::AVG: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);

                conditional_write = f_output.size() + f_output.bitPacked();
                size_t count_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t sum_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t divide_cost = ExpressionCostModel<Bit>::getDivisionCost(f_output);
                aggregator_cost = group_by_cost + conditional_write + count_cost + sum_cost + divide_cost;
                break;
            }
            default:
                aggregator_cost = 0;

        }
        per_row_cost += aggregator_cost;

        ++output_ordinal;
    }

    return per_row_cost * input_row_count;
}

size_t OperatorCostModel::nestedLoopAggregateCost(const NestedLoopAggregate<Bit> *aggregate) {
    // Cost : Input_Row_Count * Output_Cardinality * Agg_Cost_Per_Row(=per_row_cost)

    QuerySchema input_schema = aggregate->getChild()->getOutputSchema();
    QuerySchema output_schema = aggregate->getOutputSchema();

    size_t input_row_count = aggregate->getChild()->getOutputCardinality();
    size_t output_cardinality = aggregate->getOutputCardinality();

    auto group_by_defs = aggregate->group_by_;
    size_t group_by_cost = 0L;
    for(auto group_by : group_by_defs) {
        QueryFieldDesc f = input_schema.getField(group_by);
        group_by_cost += 2 *( f.size() + f.bitPacked());
    }
    group_by_cost += 4;

    int output_ordinal = group_by_defs.size();
    // ScalarAggregate : Without GroupBy Clause,
    // Cost : Input_Row_Count * Agg_Cost_Per_Row(=per_row_cost)
    auto agg_defs = aggregate->aggregate_definitions_;
    size_t per_row_cost = 0L;

    for(auto agg_def : agg_defs) {
        int ordinal = agg_def.ordinal;
        QueryFieldDesc f = input_schema.getField(ordinal);
        AggregateId agg_type = agg_def.type;

        size_t conditional_write;
        size_t initialize_cost;
        size_t aggregator_cost;

        switch(agg_type) {
            case AggregateId::MIN:
            case AggregateId::MAX: {
                conditional_write = f.size() + f.bitPacked();

                initialize_cost = f.size() + f.bitPacked();

                size_t compare_cost = ExpressionCostModel<Bit>::getComparisonCost(f) + 2;
                aggregator_cost = group_by_cost + conditional_write + initialize_cost + compare_cost + 4;
                break;
            }
            case AggregateId::COUNT: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);
                conditional_write = f_output.size() + f_output.bitPacked();

                initialize_cost = f_output.size() + f_output.bitPacked();
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = group_by_cost + conditional_write + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::SUM: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);
                conditional_write = f_output.size() + f_output.bitPacked();

                initialize_cost = 2 * (f_output.size() + f_output.bitPacked());
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = group_by_cost + 2 * conditional_write + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::AVG: {
                QueryFieldDesc f_output = output_schema.getField(output_ordinal);

                conditional_write = f_output.size() + f_output.bitPacked();
                size_t count_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t sum_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t divide_cost = ExpressionCostModel<Bit>::getDivisionCost(f_output);
                aggregator_cost = group_by_cost + conditional_write + count_cost + sum_cost + divide_cost;
                break;
            }
            default:
                aggregator_cost = 0;

        }
        per_row_cost += aggregator_cost;

        ++output_ordinal;
    }

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
        if(expression->exprClass() == ExpressionClass::GENERIC) {
            ExpressionNode<Bit> *root = ((GenericExpression<Bit> *) expression)->root_;
            ExpressionCostModel<Bit> cost_model(root, input_schema);
            row_cost += cost_model.cost();
        } // all other expression types not yet implemented, set aside for now
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
         * to_accumulate = (!input_dummy) & (!not_initialized_);
         * B to_initialize = (!input_dummy) & not_initialized_;
        */
        size_t conditional_write;
        size_t initialize_cost;
        size_t aggregator_cost;

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
                aggregator_cost = conditional_write + initialize_cost + compare_cost;
                break;
            }
            case AggregateId::COUNT: {
                QueryFieldDesc f_output = output_schema.getField(agg_idx);
                conditional_write = f_output.size() + f_output.bitPacked();

                /* initialize_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_initialize, one, output_field);
                */
                initialize_cost = f_output.size() + f_output.bitPacked();

                /* add_subtraction_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_accumulate, accumulated + one, accumulated);
                */
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = conditional_write + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::SUM: {
                QueryFieldDesc f_output = output_schema.getField(agg_idx);
                conditional_write = f_output.size() + f_output.bitPacked();

                /* initialize_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_initialize, input_field, output_field);
                */
                initialize_cost = 2 * (f_output.size() + f_output.bitPacked());

                /* add_subtraction_cost in scalar_aggregate_impl.cpp
                 * accumulated = Field<B>::If(to_accumulate, accumulated + input_field, accumulated);
                */
                size_t add_subtraction_cost = ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                aggregator_cost = conditional_write + initialize_cost + add_subtraction_cost;
                break;
            }
            case AggregateId::AVG: {
                QueryFieldDesc f_output = output_schema.getField(agg_idx);

                conditional_write = f_output.size() + f_output.bitPacked();
                size_t count_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t sum_cost = f_output.size() + f_output.bitPacked() + ExpressionCostModel<Bit>::getAddSubtractionCost(f_output);
                size_t divide_cost = ExpressionCostModel<Bit>::getDivisionCost(f_output);
                aggregator_cost = conditional_write + count_cost + sum_cost + divide_cost;
                break;
            }
            default:
                aggregator_cost = 0;

        }
        // not_initialized_ = (to_initialize & !not_initialized_) | (to_accumulate & not_initialized_);
        aggregator_cost += 4;

        per_row_cost += aggregator_cost;

        ++agg_idx;
    }

    size_t row_count = aggregate->getChild()->getOutputCardinality();
    return per_row_cost * row_count;
}

size_t OperatorCostModel::compareSwapCost(const QuerySchema &schema, const SortDefinition &sort, const int &tuple_cnt) {
    float comparison_cost = 0;

    for(auto col_sort : sort) {
        QueryFieldDesc f = schema.getField(col_sort.first);
        comparison_cost +=  f.size(); //ExpressionCostModel<Bit>::getComparisonCost(f);
        comparison_cost += 3; // bookkeeping overhead.  Bit::select (+1), & (+1)
    }

    // Add swap_cost as row width. Needs to change whole row. 
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

size_t OperatorCostModel::mergeInputCost(const MergeInput *input) {
    int tuple_cnt = input->getOutputCardinality();
    int tuple_len = input->getOutputSchema().size();

    // do two conditional writes of length {row bits} plus ANDing the dummy tags
    return tuple_cnt * (2*tuple_len + 1);
}


