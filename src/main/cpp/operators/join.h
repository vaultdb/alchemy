//
// Created by Jennie Rogers on 9/13/20.
//

#ifndef _JOIN_H
#define _JOIN_H

#include <operators/support/binary_predicate.h>
#include "operator.h"


class Join  : public Operator{


public:
    Join(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs);

protected:
    static QuerySchema concatenateSchemas(const QuerySchema & lhsSchema, const QuerySchema & rhsSchema);
    static QueryTuple concatenateTuples( QueryTuple * lhs,  QueryTuple * rhs);
    QueryTuple compareTuples(QueryTuple *lhs, QueryTuple *rhs, const types::Value &predicateEval);


    // predicate function needs aware of encrypted or plaintext state of its inputs
    std::shared_ptr<BinaryPredicate> predicate;


};


#endif //_JOIN_H
