#include "expression/visitor/join_equality_condition_visitor.h"
#include <utility>
using namespace vaultdb;


template<typename B>
std::vector<std::pair<uint32_t, uint32_t> > JoinEqualityConditionVisitor<B>::getEqualities() {
    return equalities_;
}

template<typename B>
void JoinEqualityConditionVisitor<B>::visit(EqualNode<B> & node) {
    node.lhs_->accept(this);
    uint32_t lhs_ordinal = last_ordinal_;

    node.rhs_->accept(this);
    uint32_t rhs_ordinal = last_ordinal_;

    // store them in sorted order
    if(lhs_ordinal > rhs_ordinal) {
        uint32_t t = lhs_ordinal;
        lhs_ordinal = rhs_ordinal;
        rhs_ordinal = t;
    }

    std::pair<uint32_t,uint32_t> entry(lhs_ordinal, rhs_ordinal);
    equalities_.template emplace_back(entry);
}

template<typename B>
void JoinEqualityConditionVisitor<B>::visit(AndNode<B> & node) {
    node.lhs_->accept(this);
    node.rhs_->accept(this);
}

template<typename B>
void JoinEqualityConditionVisitor<B>::visit(InputReference<B> & node) {
    last_ordinal_ = node.output_idx_;
}

template<typename B>
void JoinEqualityConditionVisitor<B>::visit(PackedInputReference<B> & node) {
    last_ordinal_ = node.output_idx_;
}
template class vaultdb::JoinEqualityConditionVisitor<bool>;
template class vaultdb::JoinEqualityConditionVisitor<emp::Bit>;

