#include "operators/multiple_union.h"
#include "operators/basic_join.h"
#include "operators/union.h"
#include "operators/secure_sql_input.h"
#include <expression/expression_factory.h>
#include <expression/visitor/type_validation_visitor.h>
#include <stdexcept>

using namespace vaultdb;

// Constructor implementation

template<typename B>
MultipleUnion<B>::MultipleUnion(const std::string &db,
                                const std::string &sql1,
                                const std::string &sql2,
                                const std::vector<int> &filter,
                                const SortDefinition &sort_def)
        : db_(db), sql1_(sql1), sql2_(sql2), filter_(filter), collation_(sort_def) {
    this->sort_definition_ = sort_def;

    // temp operator to infer schema
    auto *temp_input1 = new SecureSqlInput(db_, sql1 + std::to_string(filter.at(0)), true, collation_);
    auto *temp_input2 = new SecureSqlInput(db_, sql2 + std::to_string(filter.at(0)), true, collation_);
    this->output_schema_ = QuerySchema::concatenate(temp_input1->getOutputSchema(), temp_input2->getOutputSchema());
    this->setOutputCardinality(temp_input1->getOutputCardinality());

    delete temp_input1;
    delete temp_input2;
}


template<typename B>
QueryTable<B> *MultipleUnion<B>::runSelf() {
    Operator <B> *accumulated_union = nullptr;

    // Generate fixed join condition outside loop
    auto *temp_input1 = new SecureSqlInput(db_, sql1_ + std::to_string(filter_.at(0)), true, collation_);
    auto *temp_input2 = new SecureSqlInput(db_, sql2_ + std::to_string(filter_.at(0)), true, collation_);

    QuerySchema lhs_schema = temp_input1->getOutputSchema();
    QuerySchema rhs_schema = temp_input2->getOutputSchema();

    std::vector<ExpressionNode<B> *> operands;
    operands.push_back(new InputReference<B>(0, lhs_schema, rhs_schema));  // m.patient_id
    operands.push_back(new InputReference<B>(lhs_schema.getFieldCount(), lhs_schema, rhs_schema));  // d.patient_id

    ExpressionNode<B> *expr_root = ExpressionFactory<B>::getExpressionNode("EQUALS", operands);
    QuerySchema input_schema = QuerySchema::concatenate(lhs_schema, rhs_schema);
    TypeValidationVisitor<B> validator(expr_root, input_schema);
    expr_root->accept(&validator);
    Expression<B> *join_condition = new GenericExpression<B>(expr_root, input_schema);

    delete temp_input1;
    delete temp_input2;
    if (filter_.empty()) {
        throw std::runtime_error("MultipleUnion has no input after filtering.");
    }

    QueryTable<B> *accumulated_result = nullptr;

    if constexpr (std::is_same<B, emp::Bit>::value) {
        for (int pid: filter_) {
            // Secure inputs
            SecureSqlInput input1(db_, sql1_ + std::to_string(pid), true, collation_);
            SecureSqlInput input2(db_, sql2_ + std::to_string(pid), true, collation_);

            QueryTable<B> *lhs = input1.getOutput();
            lhs->pinned_ = true;
            QueryTable<B> *rhs = input2.getOutput();

            // Build output table for this join
            QueryTable<B> *join_result = QueryTable<B>::getTable(
                    lhs->tuple_cnt_ * rhs->tuple_cnt_,
                    this->output_schema_,
                    this->sort_definition_
            );

            int cursor = 0;
            int rhs_col_offset = this->output_schema_.getFieldCount() - rhs->getSchema().getFieldCount();

            for (int i = 0; i < lhs->tuple_cnt_; ++i) {
                B lhs_dummy = lhs->getDummyTag(i);
                join_result->cloneRowRange(cursor, 0, lhs, i, rhs->tuple_cnt_);
                join_result->cloneTable(cursor, rhs_col_offset, rhs);

                for (int j = 0; j < rhs->tuple_cnt_; ++j) {
                    B selected = join_condition->call(lhs, i, rhs, j).template getValue<B>();
                    B rhs_dummy = rhs->getDummyTag(j);
                    B result_dummy = (!selected) | lhs_dummy | rhs_dummy;
                    join_result->setDummyTag(cursor, result_dummy);
                    ++cursor;
                }
            }

            lhs->pinned_ = false;

            // Append to final output
            if (accumulated_result == nullptr) {
                accumulated_result = join_result;
            } else {
                QueryTable<B> *merged = QueryTable<B>::getTable(
                        accumulated_result->tuple_cnt_ + join_result->tuple_cnt_,
                        this->output_schema_,
                        this->sort_definition_
                );
                merged->cloneTable(0, accumulated_result);
                merged->cloneTable(accumulated_result->tuple_cnt_, join_result);
                delete accumulated_result;
                delete join_result;
                accumulated_result = merged;
            }
        }
    }

    this->output_ = accumulated_result;
    this->output_cardinality_ = accumulated_result->tuple_cnt_;
    return accumulated_result;
}



// Explicit template instantiations

template class vaultdb::MultipleUnion<bool>;
template class vaultdb::MultipleUnion<emp::Bit>;
