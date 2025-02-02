#ifndef _BLOCK_NESTED_LOOP_JOIN_H
#define _BLOCK_NESTED_LOOP_JOIN_H

#include "operators/join.h"
#include <utility>
#include <vector>


namespace vaultdb {
    template<typename B>
    class BlockNestedLoopJoin : public Join<B> {

    public:
        BlockNestedLoopJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate, const SortDefinition &sort = SortDefinition());
        BlockNestedLoopJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate, const SortDefinition &sort = SortDefinition());
        BlockNestedLoopJoin(const BlockNestedLoopJoin<B> &src) : Join<B>(src) { this->updateCollation(); }

        Operator<B> *clone() const override {
            return new BlockNestedLoopJoin<B>(*this);
        }

        ~BlockNestedLoopJoin() = default;

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::BLOCK_NESTED_LOOP_JOIN) {
                return false;
            }

            auto rhs = dynamic_cast<const BlockNestedLoopJoin<B> &>(other);
            if (*this->predicate_ != *rhs.predicate_) {
                return false;
            }

            return this->operatorEquality(other);
        }

        void updateCollation() override {
            this->getChild(0)->updateCollation();
            this->getChild(1)->updateCollation();

            // concat lhs collation with rhs one
            this->sort_definition_.clear();
            this->sort_definition_ = this->getChild(0)->getSortOrder();
            SortDefinition rhs_sort = this->getChild(1)->getSortOrder();
            this->sort_definition_.insert(this->sort_definition_.end(), rhs_sort.begin(), rhs_sort.end());
        }

    protected:
        QueryTable<B> *runSelf() override;
        inline OperatorType getType() const override { return OperatorType::BLOCK_NESTED_LOOP_JOIN; }

        int32_t foreign_key_input_ = 0; // 0 for lhs, 1 for rhs

    private:
        BufferPoolManager & bpm_ = SystemConfiguration::getInstance().bpm_;

        void joinBlock(QueryTable<Bit> *fk_table, QueryTable<Bit> *pk_table, int start_row, int row_cnt, vector<pair<uint32_t, uint32_t> > & join_idxs);
        void pinRowRange(QueryTable<Bit> *table, const int & col_ordinal, const int & start_row, const int & row_cnt);
        void unpinRowRange(QueryTable<Bit> *table, const int & col_ordinal, const int & start_row, const int & row_cnt);

        void joinRowRange(QueryTable<B> *fk_table, QueryTable<B> *pk_table, int start_row, int row_cnt);
    };
} // namespace vaultdb
#endif
