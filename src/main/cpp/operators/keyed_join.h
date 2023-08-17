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
        KeyedJoin(const KeyedJoin<B> & src) : Join<B>(src) {
            foreign_key_input_ = src.foreignKeyChild();
        }

        void updateCollation() override {
            this->getChild(0)->updateCollation();
            this->getChild(1)->updateCollation();
            // if FK is lhs, rhs has no dupes so no propagated sort order (but could propagate if needed, it just won't do anything)
            if(foreign_key_input_ == 0) {
                this->sort_definition_ = this->getChild(0)->getSortOrder();
            }
            else {
                this->sort_definition_.clear();
                int lhs_col_cnt = this->getChild(0)->getOutputSchema().getFieldCount();
                auto rhs_sort = this->getChild(1)->getSortOrder();
                for(auto col : rhs_sort) {
                    auto col_sort = ColumnSort(col.first + lhs_col_cnt, col.second);
                    this->sort_definition_.push_back(col_sort);
                }
            }

        }

        Operator<B> *clone() const override {
            assert(this->predicate_ != nullptr);
            return new KeyedJoin<B>(*this);
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
