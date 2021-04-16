#include "basic_join.h"

template<typename B>
BasicJoin<B>::BasicJoin(Operator<B> *lhs, Operator<B> *rhs, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(lhs, rhs, predicateClass) {}

template<typename B>
BasicJoin<B>::BasicJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> >rhs, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(lhs, rhs, predicateClass) {}

template<typename B>
shared_ptr<QueryTable<B> > BasicJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children[0]->getOutput();
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children[1]->getOutput();
    uint32_t cursor = 0;
    B predicateEval;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = *lhs->getSchema();
    QuerySchema rhsSchema = *rhs->getSchema();
    QuerySchema outputSchema = Join<B>::concatenateSchemas(lhsSchema, rhsSchema, false);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, is_encrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(outputTupleCount, outputSchema));

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        QueryTuple<B> lhs_tuple = (*lhs)[i];
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            QueryTuple<B> rhs_tuple = (*rhs)[j];
            predicateEval = Join<B>::predicate->predicateCall(lhs_tuple, rhs_tuple);
            B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicateEval);
            QueryTuple<B> out = (*Join<B>::output)[cursor];
            Join<B>::write_left(true, out, lhs_tuple); // all writes happen because we do the full cross product
            Join<B>::write_right(true, out, rhs_tuple);
            out.setDummyTag(dst_dummy_tag);
            ++cursor;
        }
    }
    return Join<B>::output;
}


template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;