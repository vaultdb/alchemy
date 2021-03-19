#ifndef _JOIN_H
#define _JOIN_H

#include <operators/support/binary_predicate.h>
#include "operator.h"

namespace  vaultdb {
    template<typename  B>
    class Join : public Operator<B> {


    public:
        Join(Operator<B> *lhs, Operator<B> *rhs, shared_ptr<BinaryPredicate<B> > predicateClass);
        Join(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, shared_ptr<BinaryPredicate<B> > &predicateClass);
        ~Join()  = default;
    protected:
        static QuerySchema concatenateSchemas(const QuerySchema &lhsSchema, const QuerySchema &rhsSchema);

        static QueryTuple<B> concatenateTuples(QueryTuple<B> *lhs, QueryTuple<B> *rhs);

        static QueryTuple<B> compareTuples(QueryTuple<B> *lhs, QueryTuple<B> *rhs, const B & predicateEval);


        // predicate function needs aware of encrypted or plaintext state of its inputs
        // T = BoolField || SecureBoolField
        std::shared_ptr<BinaryPredicate<B> > predicate;


    };

}



#endif //_JOIN_H
