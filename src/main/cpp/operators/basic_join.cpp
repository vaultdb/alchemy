//
// Created by Jennie Rogers on 9/13/20.
//

#include "basic_join.h"

BasicJoin::BasicJoin(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs)  : Join(predicateClass, lhs, rhs) {}

std::shared_ptr<QueryTable> BasicJoin::runSelf() {
    std::shared_ptr<QueryTable> lhs = children[0]->getOutput();
    std::shared_ptr<QueryTable> rhs = children[1]->getOutput();
    uint32_t cursor = 0;
    QueryTuple *lhsTuple, *rhsTuple;


    std::cout << "LHS: " << *lhs << std::endl;
    std::cout << "RHS: " << *rhs << std::endl;

    uint32_t outputTupleCount = lhs->getTupleCount() * rhs->getTupleCount();
    QuerySchema lhsSchema = lhs->getSchema();
    QuerySchema rhsSchema = rhs->getSchema();
    std::cout << "Concatenating schema!" << std::endl;
    QuerySchema outputSchema = concatenateSchemas(lhsSchema, rhsSchema);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC for now

    // output size, colCount, isEncrypted
    output = std::shared_ptr<QueryTable>(new QueryTable(outputTupleCount, outputSchema.getFieldCount(), lhs->isEncrypted() | rhs->isEncrypted()));
    output->setSchema(outputSchema);


    std::cout << "Done setup!" << std::endl;

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
        lhsTuple = lhs->getTuplePtr(i);
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            rhsTuple = rhs->getTuplePtr(j);
            QueryTuple dstTuple = compareTuples(lhsTuple, rhsTuple);
            output->putTuple(cursor, dstTuple);
            ++cursor;
        }
    }
    return output;
}

