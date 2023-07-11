#include "expression_cost_model.h"
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>
#include <expression/case_node.h>


using namespace vaultdb;

template<typename B>
void ExpressionCostModel<B>::visit(InputReference<B> &node) {
    last_field_desc_ = input_schema_.getField(node.read_idx_);
}

template<typename B>
void ExpressionCostModel<B>::visit(PackedInputReference<B> &node) {
    last_field_desc_ = input_schema_.getField(node.read_idx_);
}

template<typename B>
void ExpressionCostModel<B>::visit(LiteralNode<B> &node) {
    Field<B> field = node.payload_;
    // ordinal 65536 is a placeholder
    last_field_desc_ = QueryFieldDesc(65536, "anon", "anon_table", field.getType(), field.string_length_);

}

template<typename B>
void ExpressionCostModel<B>::visit(NotNode<B> &node) {
    node.lhs_->accept(this);// NOT gates computed locally
}


template<typename B>
void ExpressionCostModel<B>::visit(TimesNode<B> &node) {
    node.lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    QueryFieldDesc left_field_desc = last_field_desc_;

    node.rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    QueryFieldDesc right_field_desc = last_field_desc_;
    cumulative_cost_ = left_cost + right_cost;

    assert(left_field_desc.getType() == right_field_desc.getType());
    // take min, literals to be converted to Packed during TypeValidationVisitor
    int field_size = TypeUtilities::getTypeSize(left_field_desc.getType());
    if(right_field_desc.bitPacked() || left_field_desc.bitPacked()) {
        field_size = min(left_field_desc.size(), right_field_desc.size()) + 1;
    }

    switch(left_field_desc.getType()) {
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            cumulative_cost_ += field_size * field_size;
            break;
        case FieldType::SECURE_FLOAT:
            cumulative_cost_ += 3571;
            break;
        default:
            throw; // all others not supported
    }
}


template<typename B>
void ExpressionCostModel<B>::visit(DivideNode<B> &node) {
    node.lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    QueryFieldDesc left_field_desc = last_field_desc_;

    node.rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    QueryFieldDesc right_field_desc = last_field_desc_;
    cumulative_cost_ = left_cost + right_cost;

    assert(left_field_desc.getType() == right_field_desc.getType());
    if(right_field_desc.bitPacked() || left_field_desc.bitPacked()) {
        throw std::invalid_argument("Bit packed division yet implemented!");
    }

    switch(left_field_desc.getType()) {
        case FieldType::SECURE_INT:
            cumulative_cost_ += 1212;
        case FieldType::SECURE_LONG:
            cumulative_cost_ += 4476;
            break;
        case FieldType::SECURE_FLOAT:
            cumulative_cost_ += 3809;
            break;
        default:
            throw; // all others not supported
    }


}

template<typename B>
void ExpressionCostModel<B>::visit(CaseNode<B> &node) {
    // add up cost of expression and children
    node.conditional_.root_->accept(this);
    size_t expr_cost = cumulative_cost_;
    node.lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    node.rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    cumulative_cost_ = expr_cost + left_cost + right_cost;
}

template<typename B>
size_t ExpressionCostModel<B>::sumChildCosts(ExpressionNode<B> *node) {
    node->lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    node->rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    return left_cost + right_cost;
}

template<typename B>
void ExpressionCostModel<B>::add_subtract(ExpressionNode<B> *node) {
    node->lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    QueryFieldDesc left_field_desc = last_field_desc_;

    node->rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    QueryFieldDesc right_field_desc = last_field_desc_;

    assert(left_field_desc.getType() == right_field_desc.getType());
    // take min, literals to be converted to Packed during TypeValidationVisitor
    int field_size = left_field_desc.size();
    if(right_field_desc.bitPacked() || left_field_desc.bitPacked()) {
//        field_size = min(left_field_desc.size(), right_field_desc.size()) + 1; // +1 for sign bit
        cumulative_cost_ += field_size;
        return;
    }

    cumulative_cost_ = left_cost + right_cost + getAddSubtractionCost(left_field_desc);

}

template<typename B>
void ExpressionCostModel<B>::comparison(ExpressionNode<B> *node) {
    // float: 213
    // intN: N gates

    node->lhs_->accept(this);
    size_t left_cost = cumulative_cost_;
    QueryFieldDesc left_field_desc = last_field_desc_;

    node->rhs_->accept(this);
    size_t right_cost = cumulative_cost_;
    QueryFieldDesc right_field_desc = last_field_desc_;
    cumulative_cost_ = left_cost + right_cost;

    assert(left_field_desc.getType() == right_field_desc.getType());

    int field_size = TypeUtilities::getTypeSize(left_field_desc.getType());
    if(right_field_desc.bitPacked() || left_field_desc.bitPacked()) {
        size_t field_size = min(left_field_desc.size(), right_field_desc.size()) + 1;
        cumulative_cost_ += field_size;
        return;
    }

   cumulative_cost_ +=  getComparisonCost(left_field_desc); // both field descs are the same

}

template class vaultdb::ExpressionCostModel<bool>;
template class vaultdb::ExpressionCostModel<emp::Bit>;
