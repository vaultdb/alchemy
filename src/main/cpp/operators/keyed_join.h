#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// primary key - foreign key join
// produces an output of size |foreign key| relation
namespace  vaultdb {
    template<typename B>
    class KeyedJoin : public Join<B> {

    public:
        KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        // fkey = 0 --> lhs, fkey = 1 --> rhs
        KeyedJoin(Operator<B> *lhs, Operator<B> *rhs,  const int & fkey, const BoolExpression<B> & predicate,const SortDefinition & sort = SortDefinition());

        KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const int & fkey, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());

        ~KeyedJoin() = default;

    protected:
        std::shared_ptr<QueryTable<B> > runSelf() override;
        string getOperatorType() const override;
        int32_t foreign_key_input_ = 0; // default: lhs = fkey

    private:
        std::shared_ptr<QueryTable<B> > foreignKeyPrimaryKeyJoin();
        std::shared_ptr<QueryTable<B> > primaryKeyForeignKeyJoin();
    };

}
#endif // _PKEY_FKEY_JOIN_H
