#ifndef _JOIN_H
#define _JOIN_H

#include <expression/generic_expression.h>
#include <operators/operator.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <string>

namespace  vaultdb {
    template<typename  B>
class Join : public Operator<B> {



public:
    Join(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
    Join(QueryTable<B> *lhs, QueryTable<B> *rhs,  Expression<B> * predicate, const SortDefinition & sort = SortDefinition());
    Join(const Join<B> & src) : Operator<B>(src), predicate_(src.predicate_->clone()) {}
    ~Join() { if(predicate_ != nullptr) delete predicate_; }

    Expression<B> *getPredicate() const { return predicate_; }


    int getSourceOrdinal(const int & output_ordinal) const override {
        auto lhs_field_cnt = this->lhs_child_->getOutputSchema().getFieldCount();
        if (output_ordinal < lhs_field_cnt)
            return output_ordinal;
        return output_ordinal - lhs_field_cnt;
    }

    Operator<B> *getSourceOperator(const int & output_ordinal) const override {
        if(output_ordinal < this->lhs_child_->getOutputSchema().getFieldCount())
            return this->lhs_child_;
        return this->rhs_child_;
    }

    int getDestOrdinal(Operator<B> *src, const int & src_ordinal) const override {
        if(*src == *this->lhs_child_)
            return src_ordinal;
        assert(*src == *this->rhs_child_);

        return src_ordinal + this->lhs_child_->getOutputSchema().getFieldCount();
    }

protected:


    string getParameters() const override;


    // predicate function needs aware of encrypted or plaintext state of its inputs
    // B = BoolField || SecureBoolField
    Expression<B>  *predicate_ = nullptr;


};

} // namespace vaultdb



#endif //_JOIN_H
