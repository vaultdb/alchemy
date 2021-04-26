#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// primary key - foreign key join
// produces an output of size |foreign key| relation
namespace  vaultdb {
    template<typename B>
    class KeyedJoin : public Join<B> {

    public:
        KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, const BoolExpression<B> & predicate);
        KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate);
        ~KeyedJoin() = default;
        std::shared_ptr<QueryTable<B> > runSelf() override;

    };

}
#endif // _PKEY_FKEY_JOIN_H
