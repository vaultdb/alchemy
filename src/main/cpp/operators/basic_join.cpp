#include "basic_join.h"

using namespace vaultdb;

template<typename B>
BasicJoin<B>::BasicJoin(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {}

template<typename B>
BasicJoin<B>::BasicJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> >rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {}

template<typename B>
shared_ptr<QueryTable<B> > BasicJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children_[0]->getOutput();
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children_[1]->getOutput();
    B predicate_eval;

    QuerySchema lhs_schema = *lhs->getSchema();
    QuerySchema rhs_schema = *rhs->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    SortDefinition concat_sorts;
    concat_sorts = lhs->getSortOrder();
    SortDefinition  rhs_sort = rhs->getSortOrder();
    concat_sorts.insert(concat_sorts.end(),  rhs_sort.begin(), rhs_sort.end());

    B selected, dst_dummy_tag;
    // output size, colCount, is_encrypted
    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(lhs->getTupleCount() * rhs->getTupleCount(), output_schema, concat_sorts));
    int cursor = 0;

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        QueryTuple<B> lhs_tuple = (*lhs)[i];
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            QueryTuple<B> rhs_tuple = (*rhs)[j];

            QueryTuple<B> out = (*Join<B>::output_)[cursor];
            Join<B>::write_left(out, lhs_tuple); // all writes happen because we do the full cross product
            Join<B>::write_right(out, rhs_tuple);

            selected = Join<B>::predicate_->call(out).template getValue<B>();
            dst_dummy_tag = (!selected) | lhs_tuple.getDummyTag() | rhs_tuple.getDummyTag();
            out.setDummyTag(dst_dummy_tag);
            ++cursor;
        }
    }

    return Join<B>::output_;
}



template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;