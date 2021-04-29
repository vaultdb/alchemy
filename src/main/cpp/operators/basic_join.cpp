#include "basic_join.h"

using namespace vaultdb;

template<typename B>
BasicJoin<B>::BasicJoin(Operator<B> *lhs, Operator<B> *rhs, const BoolExpression<B> & predicate)
        : Join<B>(lhs, rhs, predicate) {}

template<typename B>
BasicJoin<B>::BasicJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> >rhs, const BoolExpression<B> & predicate)
        : Join<B>(lhs, rhs, predicate) {}

template<typename B>
shared_ptr<QueryTable<B> > BasicJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children_[0]->getOutput();
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children_[1]->getOutput();
    uint32_t cursor = 0;
    B predicate_eval;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = *lhs->getSchema();
    QuerySchema rhsSchema = *rhs->getSchema();
    QuerySchema outputSchema = Join<B>::concatenateSchemas(lhsSchema, rhsSchema, false);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    SortDefinition concat_sorts;
    concat_sorts = lhs->getSortOrder();
    SortDefinition  rhs_sort = rhs->getSortOrder();
    std::cout << "Join received " << concat_sorts.size() << " col sorts from lhs " << std::endl;
    concat_sorts.insert(concat_sorts.end(),  rhs_sort.begin(), rhs_sort.end());
    std::cout << "Join received " << rhs_sort.size() << " col sorts from rhs." << std::endl;

    // output size, colCount, is_encrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(outputTupleCount, outputSchema, concat_sorts));

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        QueryTuple<B> lhs_tuple = (*lhs)[i];
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            QueryTuple<B> rhs_tuple = (*rhs)[j];

            QueryTuple<B> out = (*Join<B>::output)[cursor];
            Join<B>::write_left(true, out, lhs_tuple); // all writes happen because we do the full cross product
            Join<B>::write_right(true, out, rhs_tuple);

            predicate_eval = Join<B>::predicate_.callBoolExpression(out);
            B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);
            out.setDummyTag(dst_dummy_tag);
            ++cursor;
        }
    }

    std::cout << "Join has output sort order with " << Join<B>::output->getSortOrder().size()  << " cols." <<  std::endl;
    return Join<B>::output;
}


template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;