#include "basic_join.h"
#include "query_table/table_factory.h"

using namespace vaultdb;

template<typename B>
BasicJoin<B>::BasicJoin(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {}

template<typename B>
BasicJoin<B>::BasicJoin(QueryTable<B> *lhs, QueryTable<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {}

template<typename B>
QueryTable<B> *BasicJoin<B>::runSelf() {
    QueryTable<B> *lhs = Operator<B>::getChild(0)->getOutput();
    QueryTable<B> *rhs = Operator<B>::getChild(1)->getOutput();
    B predicate_eval;


    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    SortDefinition concat_sorts;
    concat_sorts = lhs->getSortOrder();
    SortDefinition  rhs_sort = rhs->getSortOrder();
    concat_sorts.insert(concat_sorts.end(),  rhs_sort.begin(), rhs_sort.end());

    B selected, dst_dummy_tag, lhs_dummy_tag;
    // output size, colCount, is_encrypted
    this->output_ = TableFactory<B>::getTable(lhs->getTupleCount() * rhs->getTupleCount(), this->output_schema_, lhs->storageModel(), concat_sorts);
    int cursor = 0;

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
         lhs_dummy_tag  = lhs->getDummyTag(i);
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            Join<B>::write_left(Operator<B>::output_, cursor,  lhs, i);
            Join<B>::write_right(Operator<B>::output_, cursor,  rhs, j);

            selected = Join<B>::predicate_->call(Operator<B>::output_, cursor).template getValue<B>();
            dst_dummy_tag = (!selected) | lhs_dummy_tag | rhs->getDummyTag(j);
            Operator<B>::output_->setDummyTag(cursor, dst_dummy_tag);
            ++cursor;
        }
    }

    return this->output_;
}



template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;