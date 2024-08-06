#ifndef _MERGE_JOIN_H
#define _MERGE_JOIN_H

#include "operators/join.h"
#include "expression/expression.h"

namespace  vaultdb {

template<typename B>
class MergeJoin : public Join<B> {
    public:
    // predicate unused, columns lined up owing to construction in input order
    MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
        setup();
      }

    MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
        setup();
    }

    MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool or_dummy_tags = false) : Join<B>(lhs, rhs, predicate, sort), or_dummy_tags_(or_dummy_tags) {
        setup();
    }

    MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool or_dummy_tags = false) : Join<B>(lhs, rhs, predicate, sort), or_dummy_tags_(or_dummy_tags)  {
        setup();
        }

   MergeJoin(const MergeJoin & src) : Join<B>(src) {
            or_dummy_tags_ = src.or_dummy_tags_;
   }

    Operator<B> *clone() const override {
         return new MergeJoin<B>(*this);
    }

    void updateCollation() override {
        this->getChild(0)->updateCollation();
        this->getChild(1)->updateCollation();

        // just use lhs sort order for now, can use one or sides' since they are equivalent for equi-join
        this->sort_definition_ = this->getChild()->getSortOrder();
        auto lhs = this->getChild(0);
        auto rhs = this->getChild(1);

        // check sort compatibility
        auto lhs_collation = lhs->getSortOrder();
        auto rhs_collation = rhs->getSortOrder();
        assert(lhs_collation.size() == rhs_collation.size());
        for(size_t i = 0; i < lhs_collation.size(); ++i) {
            assert(lhs_collation[i] == rhs_collation[i]);
        }

    }

    bool operator==(const Operator<B> & other) const override {
        if (other.getType() != OperatorType::MERGE_JOIN) {
            return false;
        }

        auto other_node = dynamic_cast<const MergeJoin<B> &>(other);
        if(this->or_dummy_tags_ != other_node.or_dummy_tags_) return false;

        return this->operatorEquality(other);
    }

    bool getDummyHandling() const { return or_dummy_tags_; }

protected:
    bool or_dummy_tags_ = false;
    QueryTable<B> *runSelf() override;
    inline OperatorType getType() const override { return OperatorType::MERGE_JOIN; }


private:
    void setup();
};




} // namespace vaultdb


#endif
