//
// Created by Jennie Rogers on 9/21/20.
//

#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// produces an output of size |foreign key| relation
class ForeignKeyPrimaryKeyJoin : public Join {

public:
    ForeignKeyPrimaryKeyJoin(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &foreignKey, std::shared_ptr<Operator> &primaryKey);
    std::shared_ptr<QueryTable> runSelf() override;

};


#endif // _PKEY_FKEY_JOIN_H
