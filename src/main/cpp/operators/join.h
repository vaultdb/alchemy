#ifndef _JOIN_H
#define _JOIN_H

#include <operators/support/binary_predicate.h>
#include "operator.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

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

        static B get_dummy_tag(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs, const B & predicateEval);


        // predicate function needs aware of encrypted or plaintext state of its inputs
        // T = BoolField || SecureBoolField
        std::shared_ptr<BinaryPredicate<B> > predicate;

        // if B write is true, then write to the left side of an output tuple with src_tuple
        static void write_left(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_left(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);

        // if B write is true, then write to the right side of an output tuple with src_tuple
        static void write_right(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_right(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);


    };

}



#endif //_JOIN_H
