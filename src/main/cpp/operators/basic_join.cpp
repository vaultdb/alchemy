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
    QueryTuple<B> *lhsTuple, *rhsTuple;
    B predicateEval;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = lhs->getSchema();
    QuerySchema rhsSchema = rhs->getSchema();
    QuerySchema outputSchema = Join<B>::concatenateSchemas(lhsSchema, rhsSchema);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, isEncrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(outputTupleCount, outputSchema.getFieldCount()));
    Join<B>::output->setSchema(outputSchema);

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        lhsTuple = lhs->getTuplePtr(i);
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            rhsTuple = rhs->getTuplePtr(j);
            predicateEval = Join<B>::predicate->predicateCall(lhsTuple, rhsTuple);
            QueryTuple dstTuple = Join<B>::compareTuples(lhsTuple, rhsTuple, predicateEval);
            Join<B>::output->putTuple(cursor, dstTuple);
            ++cursor;
        }
    }
    return Join<B>::output;
}


template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;