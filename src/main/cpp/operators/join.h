#ifndef _JOIN_H
#define _JOIN_H

#include <expression/bool_expression.h>
#include "operator.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

namespace  vaultdb {
    template<typename  B>
    class Join : public Operator<B> {


    public:
        Join(Operator<B> *lhs, Operator<B> *rhs, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        Join(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs,  const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        ~Join()  = default;



        // if B write is true, then write to the left side of an output tuple with src_tuple
        static void write_left(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_left(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);

        // if B write is true, then write to the right side of an output tuple with src_tuple
        static void write_right(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_right(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);


    protected:
        static QuerySchema concatenateSchemas(const QuerySchema &lhs_schema, const QuerySchema &rhs_schema, const bool &append_bool = false);


        static B get_dummy_tag(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs, const B & predicateEval);


        // predicate function needs aware of encrypted or plaintext state of its inputs
        // B = BoolField || SecureBoolField
        BoolExpression<B>  predicate_;


    };

}



#endif //_JOIN_H
