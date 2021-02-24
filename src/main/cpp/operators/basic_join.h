// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "join.h"


namespace vaultdb {

    class BasicJoin : public Join {

    public:
        BasicJoin(Operator *lhs, Operator *rhs, shared_ptr<BinaryPredicate> predicateClass);
        BasicJoin(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable> rhs, std::shared_ptr<BinaryPredicate> predicateClass);
        ~BasicJoin() = default;
        std::shared_ptr<QueryTable> runSelf() override;
    };
}

#endif //_BASIC_JOIN_H
