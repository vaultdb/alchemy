#ifndef _SHRINKWRAP_H
#define _SHRINKWRAP_H

#include "operators/operator.h"
#include <algorithm>
#include <string>
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

    // based on "Fast Fully Oblivious Compaction and Shuffling" CCS 2022
    // https://eprint.iacr.org/2022/1333.pdf
    static void orOffCompact(QueryTable<B> *to_compact, const Field<B> & z, const int &start_idx, const int &end_idx);
    static void  ORCompact(QueryTable<B> *to_compact, const int & end_offset = -1); // -1 = all
    static void compact(QueryTable<B> *to_compact);
protected:
    QueryTable<B>* runSelf()  override;
    string getParameters() const override {  return "cardinality_bound=" + std::to_string(this->output_cardinality_); }
    OperatorType getType() const override { return OperatorType::SHRINKWRAP; }

};


} // namespace vaultdb
#endif //VAULTDB_EMP_SHRINKWRAP_H
