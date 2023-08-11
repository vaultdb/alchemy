#ifndef _PKEY_FKEY_JOIN_H
#define _PKEY_FKEY_JOIN_H


#include "join.h"

// primary key - foreign key join
// produces an output of size |foreign key| relation
namespace  vaultdb {
    template<typename B>
    class KeyedJoin : public Join<B> {

    public:
        KeyedJoin(Operator<B> *foreign_Key, Operator<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
        // fkey = 0 --> lhs, fkey = 1 --> rhs
        KeyedJoin(Operator<B> *lhs, Operator<B> *rhs,  const int & fkey, Expression<B> *predicate,const SortDefinition & sort = SortDefinition());

        KeyedJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
        KeyedJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, const int & fkey, Expression<B> *predicate, const SortDefinition & sort = SortDefinition());

        Operator<B> *clone() const override {
            assert(this->predicate_ != nullptr);
            return new KeyedJoin<B>(this->lhs_child_->clone(), this->rhs_child_->clone(), this->foreign_key_input_, this->predicate_->clone(), this->sort_definition_);
        }

        ~KeyedJoin() = default;
        int foreignKeyChild() const { return foreign_key_input_; }

    protected:
        QueryTable<B> *runSelf() override;

        OperatorType getType() const override {
            return OperatorType::KEYED_NESTED_LOOP_JOIN;
        }

        int32_t foreign_key_input_ = 0; // default: lhs = fkey

    private:
        QueryTable<B> *foreignKeyPrimaryKeyJoin();
        QueryTable<B> *primaryKeyForeignKeyJoin();
    };

}
#endif // _PKEY_FKEY_JOIN_H
