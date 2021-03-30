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
    QueryTuple<B> lhs_tuple, rhs_tuple;
    B predicateEval;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = *lhs->getSchema();
    QuerySchema rhsSchema = *rhs->getSchema();
    QuerySchema outputSchema = Join<B>::concatenateSchemas(lhsSchema, rhsSchema);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, isEncrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(outputTupleCount, outputSchema));

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        lhs_tuple = (*lhs)[i];
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            rhs_tuple = (*rhs)[j];
            predicateEval = Join<B>::predicate->predicateCall(lhs_tuple, rhs_tuple);
            B dst_dummy_tag = Join<B>::compareTuples(lhs_tuple, rhs_tuple, predicateEval);
            QueryTuple<B> out = (*Join<B>::output)[cursor];
            out.setDummyTag(dst_dummy_tag);
            Join<B>::write_left(dst_dummy_tag, out, lhs_tuple);
            Join<B>::write_right(dst_dummy_tag, out, rhs_tuple);
            ++cursor;
        }
    }
    return Join<B>::output;
}


template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;