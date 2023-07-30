#ifndef _SHRINKWRAP_H
#define _SHRINKWRAP_H

#include "operator.h"

// obliviously truncate a query output
// sort to put dummies at the end
namespace vaultdb {
    template<typename B>
    class Shrinkwrap : public Operator<B> {
    public:
        Shrinkwrap(Operator<B> *child, const size_t & output_cardinality);
        Shrinkwrap(QueryTable<B>*input, const size_t & output_cardinality);
        Operator<B> *clone() const override {
            return new Shrinkwrap<B>(this->lhs_child_->clone(), this->output_cardinality_);
        }

        ~Shrinkwrap() = default;

    protected:
        QueryTable<B>* runSelf()  override;
        string getOperatorType() const override {     return "Shrinkwrap"; }
        string getParameters() const override {  return "cardinality_bound=" + std::to_string(this->output_cardinality_); }


    };


}
#endif //VAULTDB_EMP_SHRINKWRAP_H
