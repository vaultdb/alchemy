#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// primary key - foreign key join
// produces an output of size |foreign key| relation
class KeyedJoin : public Join {

public:
    KeyedJoin(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &foreignKey, std::shared_ptr<Operator> &primaryKey);
    std::shared_ptr<QueryTable> runSelf() override;

};


#endif // _PKEY_FKEY_JOIN_H
