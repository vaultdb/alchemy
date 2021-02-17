#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// primary key - foreign key join
// produces an output of size |foreign key| relation
namespace  vaultdb {
    class KeyedJoin : public Join {

    public:
        KeyedJoin(Operator *foreignKey, Operator *primaryKey, shared_ptr<BinaryPredicate> predicateClass);
        KeyedJoin(shared_ptr<QueryTable> foreignKey, shared_ptr<QueryTable> primaryKey, shared_ptr<BinaryPredicate> predicateClass);

        std::shared_ptr<QueryTable> runSelf() override;

    };

}
#endif // _PKEY_FKEY_JOIN_H
