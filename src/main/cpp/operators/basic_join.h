// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "operators/join.h"
#include <string>


namespace vaultdb {

template<typename  B>
class BasicJoin : public Join<B> {

public:
    BasicJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
    BasicJoin(QueryTable<B> *lhs, QueryTable<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
    BasicJoin(const BasicJoin<B> & src) : Join<B>(src), isCartesian_(src.isCartesian_) { this->updateCollation(); }

    Operator<B> *clone() const override {
        return new BasicJoin<B>(*this);
    }

    ~BasicJoin() = default;

    bool operator==(const Operator<B> &other) const override {
        if (other.getType() != OperatorType::NESTED_LOOP_JOIN) {
            return false;
        }

        auto rhs = dynamic_cast<const BasicJoin<B> &>(other);
        if(*this->predicate_ != *rhs.predicate_) {
            return false;
        }

        return this->operatorEquality(other);
    }


    void updateCollation() override {
        this->getChild(0)->updateCollation();
        this->getChild(1)->updateCollation();

        // concat lhs collation with rhs one
        this->sort_definition_.clear();
        this->sort_definition_ = this->getChild(0)->getSortOrder();
        SortDefinition  rhs_sort = this->getChild(1)->getSortOrder();
        this->sort_definition_.insert(this->sort_definition_.end(),  rhs_sort.begin(), rhs_sort.end());
    }

    // A setter so that the parser can mark this join as cartesian.
    void setCartesian(bool flag) { isCartesian_ = flag; }

    // You can also provide a getter if needed.
    bool isCartesian() const { return isCartesian_; }

protected:
    bool isCartesian_ = false;
    QueryTable<B> *runSelf() override;
    inline OperatorType getType() const override { return OperatorType::NESTED_LOOP_JOIN; }


};
} // namespace vaultdb

#endif //_BASIC_JOIN_H
