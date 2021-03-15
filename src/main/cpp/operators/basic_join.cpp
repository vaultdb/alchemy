#include "basic_join.h"

template<typename T>
BasicJoin<T>::BasicJoin(Operator *lhs, Operator *rhs, shared_ptr<BinaryPredicate<T> > predicateClass)
        : Join<T>(lhs, rhs, predicateClass) {}

template<typename T>
BasicJoin<T>::BasicJoin(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable>rhs, shared_ptr<BinaryPredicate<T> > predicateClass)
        : Join<T>(lhs, rhs, predicateClass) {}

template<typename T>
shared_ptr<QueryTable> BasicJoin<T>::runSelf() {
    std::shared_ptr<QueryTable> lhs = Join<T>::children[0]->getOutput();
    std::shared_ptr<QueryTable> rhs = Join<T>::children[1]->getOutput();
    uint32_t cursor = 0;
    QueryTuple *lhsTuple, *rhsTuple;
    T predicateEval;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = lhs->getSchema();
    QuerySchema rhsSchema = rhs->getSchema();
    QuerySchema outputSchema = Join<T>::concatenateSchemas(lhsSchema, rhsSchema);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, isEncrypted
    Join<T>::output = std::shared_ptr<QueryTable>(new QueryTable(outputTupleCount, outputSchema.getFieldCount()));
    Join<T>::output->setSchema(outputSchema);

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        lhsTuple = lhs->getTuplePtr(i);
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            rhsTuple = rhs->getTuplePtr(j);
            predicateEval = Join<T>::predicate->predicateCall(lhsTuple, rhsTuple);
            QueryTuple dstTuple = Join<T>::compareTuples(lhsTuple, rhsTuple, predicateEval);
            Join<T>::output->putTuple(cursor, dstTuple);
            ++cursor;
        }
    }
    return Join<T>::output;
}


template class vaultdb::BasicJoin<BoolField>;
template class vaultdb::BasicJoin<SecureBoolField>;