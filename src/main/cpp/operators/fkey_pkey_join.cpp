#include <operators/support/secure_replace_tuple.h>
#include "fkey_pkey_join.h"

KeyedJoin::KeyedJoin(Operator *foreignKey, Operator *primaryKey, shared_ptr<BinaryPredicate> predicateClass)
        : Join(
        foreignKey,
        primaryKey, predicateClass) {}

KeyedJoin::KeyedJoin(shared_ptr<QueryTable> foreignKey, shared_ptr<QueryTable> primaryKey, shared_ptr<BinaryPredicate> predicateClass)
        : Join(
        foreignKey,
        primaryKey, predicateClass) {}

std::shared_ptr<QueryTable> KeyedJoin::runSelf() {
    std::shared_ptr<QueryTable> foreignKeyTable = children[0]->getOutput();
    std::shared_ptr<QueryTable> primaryKeyTable = children[1]->getOutput();
    QueryTuple *lhsTuple, *rhsTuple;
    QueryTuple dstTuple;
    types::Value predicateEval;

    uint32_t outputTupleCount = foreignKeyTable->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhsSchema = foreignKeyTable->getSchema();
    QuerySchema rhsSchema = primaryKeyTable->getSchema();
    QuerySchema outputSchema = concatenateSchemas(lhsSchema, rhsSchema);



    assert(foreignKeyTable->isEncrypted() == primaryKeyTable->isEncrypted()); // only support all plaintext or all MPC for now

    // output size, colCount, isEncrypted
    output = std::shared_ptr<QueryTable>(new QueryTable(outputTupleCount, outputSchema.getFieldCount()));
    output->setSchema(outputSchema);
    ReplaceTuple *replaceTuple = (foreignKeyTable->isEncrypted()) ? new SecureReplaceTuple(output) : new ReplaceTuple(output);


    // TODO: create dst tuple and populate with LHS value
    // TODO: (cont'd) then RHS overwrites only the second half
    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < foreignKeyTable->getTupleCount(); ++i) {
        lhsTuple = foreignKeyTable->getTuplePtr(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhsTuple = primaryKeyTable->getTuplePtr(0);
        predicateEval = predicate->predicateCall(lhsTuple, rhsTuple);
        dstTuple = compareTuples(lhsTuple, rhsTuple, predicateEval);
        // unconditional write to first one to ensure it is initialized
        output->putTuple(i, dstTuple);

        for(uint32_t j = 1; j < primaryKeyTable->getTupleCount(); ++j) {
            rhsTuple = primaryKeyTable->getTuplePtr(j);
            predicateEval = predicate->predicateCall(lhsTuple, rhsTuple);
             dstTuple = compareTuples(lhsTuple, rhsTuple, predicateEval);
            replaceTuple->conditionalWrite(i, dstTuple, predicateEval);

        }

    }

    delete replaceTuple;

    return output;

}



