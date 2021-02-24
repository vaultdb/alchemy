#ifndef _JOIN_H
#define _JOIN_H

#include <operators/support/binary_predicate.h>
#include "operator.h"

namespace  vaultdb {
    class Join : public Operator {


    public:
        Join(Operator *lhs, Operator *rhs, shared_ptr<BinaryPredicate> predicateClass);
        Join(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable> rhs, shared_ptr<BinaryPredicate> &predicateClass);
        ~Join()  = default;
    protected:
        static QuerySchema concatenateSchemas(const QuerySchema &lhsSchema, const QuerySchema &rhsSchema);

        static QueryTuple concatenateTuples(QueryTuple *lhs, QueryTuple *rhs);

        static QueryTuple compareTuples(QueryTuple *lhs, QueryTuple *rhs, const types::Value &predicateEval);


        // predicate function needs aware of encrypted or plaintext state of its inputs
        std::shared_ptr<BinaryPredicate> predicate;


    };

}

#endif //_JOIN_H
