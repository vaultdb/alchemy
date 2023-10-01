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
        Shrinkwrap(const Shrinkwrap<B> & src) : Operator<B>(src) {}
        Operator<B> *clone() const override {
            return new Shrinkwrap<B>(*this);
        }

        ~Shrinkwrap() = default;

        void updateCollation() override {
            this->getChild()->updateCollation();

            SortDefinition src_sort = this->getChild()->getSortOrder();
            SortDefinition  dst_sort;
            if(!src_sort.empty() && src_sort[0].first != -1)
                dst_sort.push_back(ColumnSort(-1, SortDirection::ASCENDING));  // not-dummies go first
            for(ColumnSort c : src_sort) {
                dst_sort.push_back(c);
            }
            this->sort_definition_ = dst_sort;
        }

        bool operator==(const Operator<B> & rhs) const override {
            if (rhs.getType() != OperatorType::SHRINKWRAP) {
                return false;
            }

            return this->operatorEquality(rhs);
        }

    protected:
        QueryTable<B>* runSelf()  override;
        string getParameters() const override {  return "cardinality_bound=" + std::to_string(this->output_cardinality_); }
        OperatorType getType() const override { return OperatorType::SHRINKWRAP; }


    };


}
#endif //VAULTDB_EMP_SHRINKWRAP_H
