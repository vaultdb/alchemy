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

            if(foreign_key_input_ == 0) {
                this->sort_definition_ = this->getChild(0)->getSortOrder();
                // lhs--> rhs
                // only append if lhs has collation
                if(!this->sort_definition_.empty()) {
                    int lhs_col_cnt = this->getChild(0)->getOutputSchema().getFieldCount();
                    for(auto col : this->sort_definition_) {
                        auto col_sort = ColumnSort(col.first + lhs_col_cnt, col.second);
                        this->sort_definition_.push_back(col_sort);
                    }
                }
            }
            else {
                // rhs --> lhs
                this->sort_definition_.clear();
                int lhs_col_cnt = this->getChild(0)->getOutputSchema().getFieldCount();
                auto rhs_sort = this->getChild(1)->getSortOrder();
                for(auto col : rhs_sort) {
                    auto col_sort = ColumnSort(col.first + lhs_col_cnt, col.second);
                    this->sort_definition_.push_back(col_sort);
                }
                if(!this->sort_definition_.empty()) {
                    this->sort_definition_.insert(this->sort_definition_.end(), this->getChild(0)->getSortOrder().begin(), this->getChild(0)->getSortOrder().end());
                }

            }

        }

        Operator<B> *clone() const override {
            assert(this->predicate_ != nullptr);
            return new KeyedJoin<B>(*this);
        }

        ~KeyedJoin() = default;
        int foreignKeyChild() const { return foreign_key_input_; }
        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::KEYED_NESTED_LOOP_JOIN) {
                return false;
            }

            auto rhs = dynamic_cast<const KeyedJoin<B> &>(other);


            if(*this->predicate_ != *rhs.predicate_ || this->foreign_key_input_ != rhs.foreign_key_input_) return false;
            return this->operatorEquality(other);
        }

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
