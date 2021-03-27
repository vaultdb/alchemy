#include "fkey_pkey_join.h"

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(foreignKey, primaryKey, predicateClass) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(foreignKey, primaryKey, predicateClass) {}



template<typename B>
std::shared_ptr<QueryTable<B> > KeyedJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > foreignKeyTable = Join<B>::children[0]->getOutput();
    std::shared_ptr<QueryTable<B> > primaryKeyTable = Join<B>::children[1]->getOutput();
    QueryTuple<B> *lhsTuple, *rhsTuple;
    QueryTuple<B> dstTuple;

    uint32_t outputTupleCount = foreignKeyTable->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhsSchema = foreignKeyTable->getSchema();
    QuerySchema rhsSchema = primaryKeyTable->getSchema();
    QuerySchema outputSchema = Join<B>::concatenateSchemas(lhsSchema, rhsSchema);



    assert(foreignKeyTable->isEncrypted() == primaryKeyTable->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, isEncrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(outputTupleCount, outputSchema.getFieldCount()));
    Join<B>::output->setSchema(outputSchema);



    // TODO: create dst tuple and populate with LHS value
    // TODO: (cont'd) then RHS overwrites only the second half
    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < foreignKeyTable->getTupleCount(); ++i) {
        lhsTuple = foreignKeyTable->getTuplePtr(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhsTuple = primaryKeyTable->getTuplePtr(0);
        B predicateEval = Join<B>::predicate->predicateCall(lhsTuple, rhsTuple);
        dstTuple = Join<B>::compareTuples(lhsTuple, rhsTuple, predicateEval);
        // unconditional write to first one to initialize it
        Join<B>::output->putTuple(i, dstTuple);

        for(uint32_t j = 1; j < primaryKeyTable->getTupleCount(); ++j) {
            rhsTuple = primaryKeyTable->getTuplePtr(j);
            predicateEval = Join<B>::predicate->predicateCall(lhsTuple, rhsTuple);
             dstTuple = Join<B>::compareTuples(lhsTuple, rhsTuple, predicateEval);
            QueryTuple<B>::compareAndSwap(predicateEval, Join<B>::output->getTuplePtr(i), &dstTuple);


        }

    }
    return Join<B>::output;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


