#include "operators/shrinkwrap.h"
#include <operators/sort.h>
#include <query_table/field/field_factory.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
Shrinkwrap<B>::Shrinkwrap(Operator<B> *child, const size_t &output_cardinality) : Operator<B>(child, child->getSortOrder()) {
    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization
    updateCollation();
}

template<typename B>
Shrinkwrap<B>::Shrinkwrap( QueryTable<B>*input, const size_t &output_cardinality)  : Operator<B>(input) {

    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization
    updateCollation();

}

// only works for powers of two, need `OrCompact` for arbitrary length tables
// moves all dummies to the front of the table
// CAUTION: this algorithm requires flipping the dummy tags to push dummies to end.
template<typename B>
void Shrinkwrap<B>::orOffCompact(QueryTable<B> *to_compact, const Field<B> &z, const int &start_idx, const int &end_idx) {


    int n = end_idx - start_idx + 1; // zero-indexed
    FieldType int_type = std::is_same_v<B, Bit> ? FieldType::SECURE_INT : FieldType::INT;
    Field<B> zero = FieldFactory<B>::getZero(int_type);
    Field<B> one = FieldFactory<B>::getOne(int_type);

    if (n == 1) return;

    if (n == 2) {
        B m0 = to_compact->getDummyTag(start_idx);
        B m1 = to_compact->getDummyTag(end_idx);
        B choice = (((!m0) & m1) ^ (z > zero));
        to_compact->compareSwap(choice, start_idx, end_idx);
        return;
    }

    Field<B> m = zero;
    // Field<B> m = getM(to_compact, start_idx, start_idx + (n/2) - 1);
    for (int i = start_idx; i < (start_idx + n/2); ++i) {
        auto to_add = Field<B>::If(to_compact->getDummyTag(i), one, zero);
        m = m + to_add;
    }

    Field<B> n_div_2 =  FieldFactory<B>::getInt(n / 2);
    auto z_mod  = (z % (n_div_2));
    auto z_m_mod = (z + m) % n_div_2;
    B left_wrapped = ((z_mod + m) >= n_div_2);
    B offset_right = (z >= n_div_2);



    orOffCompact(to_compact, z_mod, start_idx, start_idx + (n / 2) - 1);
    orOffCompact(to_compact, z_m_mod,  start_idx + (n / 2),  end_idx);


    B s = left_wrapped ^ offset_right;

    auto f_i = zero;
    // should be swapping 3, 6
    for (int i = start_idx; i < (start_idx + n/2); ++i) {
        B choice = s ^ (f_i >= z_m_mod);
        to_compact->compareSwap(choice, i, i+(n/2));
        f_i = f_i + one;
    }

}

template<typename B>
void Shrinkwrap<B>::ORCompact(QueryTable<B> *to_compact, const int & end_offset) {
    int end_idx = end_offset;
    if (end_offset == -1) {
        end_idx = to_compact->tuple_cnt_ -1;
    }

    if (end_idx == 0) {
        return;
    }

    int n = end_idx+1;
    if (n < 2) {
        return;
    }
    if (n == 2) {
        B swap = (!to_compact->getDummyTag(0) & to_compact->getDummyTag(1));
        to_compact->compareSwap(swap, 0, 1);
        return;
    }

    int min_log2 = log2(n);
    int gt_pow2 = std::pow(2, min_log2); // power of 2 lt n
    int split_idx = n - gt_pow2;

    FieldType int_type = std::is_same_v<B, Bit> ? FieldType::SECURE_INT : FieldType::INT;
    auto zero = FieldFactory<B>::getZero(int_type);
    auto one = FieldFactory<B>::getOne(int_type);
    auto m = zero;

    for (int i = 0; i < split_idx; ++i) {
        auto dummy_tag  = to_compact->getDummyTag(i);
        auto to_add = Field<B>::If(to_compact->getDummyTag(i), one, zero);
        m = m + to_add;
    }


    auto n1_field = FieldFactory<B>::getInt(gt_pow2);
    auto n2_field = FieldFactory<B>::getInt(split_idx);
    auto z =  (n1_field - n2_field + m) % n1_field;
    if (split_idx > 0) ORCompact(to_compact, split_idx-1);


    orOffCompact(to_compact, z,  split_idx, n-1);

    Field<B> i_field = FieldFactory<B>::getZero(int_type);
    for (int i = 0; i < split_idx; ++i) {
        B choice = (i_field >= m);
        to_compact->compareSwap(choice, i, i + gt_pow2);
        i_field = i_field + one;
    }

}

template<typename B>
void Shrinkwrap<B>::compact(QueryTable<B> *to_compact) {
    for (int i = 0; i < to_compact->tuple_cnt_; ++i) {
        auto dummy_tag = to_compact->getDummyTag(i);
        to_compact->setDummyTag(i, !dummy_tag);
    }
    ORCompact(to_compact);
    for (int i = 0; i < to_compact->tuple_cnt_; ++i) {
        auto dummy_tag = to_compact->getDummyTag(i);
        to_compact->setDummyTag(i, !dummy_tag);
    }

}

template<typename B>
QueryTable<B>*Shrinkwrap<B>::runSelf() {

    QueryTable<B> *input = this->getChild(0)->getOutput();


    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    if(input->tuple_cnt_ <= this->output_cardinality_) {
        Operator<B>::output_ =  input->clone();
        return this->output_;
    }

    this->output_ = input->clone();
    compact(this->output_);
    this->setSortOrder(this->sort_definition_);
    this->output_->resize(this->output_cardinality_);
    this->output_->order_by_ = this->sort_definition_;
    return this->output_;
}




template class vaultdb::Shrinkwrap<bool>;
template class vaultdb::Shrinkwrap<emp::Bit>;