// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "join.h"


namespace vaultdb {

    template<typename  T>
    class BasicJoin : public Join<T> {

    public:
        BasicJoin(Operator *lhs, Operator *rhs, shared_ptr<BinaryPredicate<T>> predicateClass);
        BasicJoin(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable> rhs, std::shared_ptr<BinaryPredicate<T>> predicateClass);
        ~BasicJoin() = default;
        std::shared_ptr<QueryTable> runSelf() override;
    };
}

#endif //_BASIC_JOIN_H
