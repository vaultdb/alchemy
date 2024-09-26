#ifndef _UNION_H
#define _UNION_H

#include "operators/operator.h"
#include <string>

// concatenates two input arrays.  Equivalent to UNION ALL in SQL
namespace  vaultdb {

template<typename B>
class Union : public Operator<B> {


public:
    // not order preserving, so no sort definition
    Union(Operator<B> *lhs, Operator<B> *rhs);

    Union(QueryTable<B> *lhs, QueryTable<B> *rhs);

    Union(const Union & src) : Operator<B>(src) { }

    Operator<B> *clone() const override {
        return new Union<B>(*this);
    }

    void updateCollation() override {
        this->getChild()->updateCollation();
        this->getChild(1)->updateCollation();
    }

    virtual ~Union() = default;

    bool operator==(const Operator<B> & other) const override {
        if (other.getType() != OperatorType::UNION) {
            return false;
        }

        return this->operatorEquality(other);
    }

protected:
    QueryTable<B> *runSelf()  override;
    OperatorType getType() const override { return OperatorType::UNION; }
    string getParameters() const override { return ""; }



};

} // namespace vaultdb



#endif //_UNION_H
