#ifndef _JOIN_H
#define _JOIN_H

#include <expression/generic_expression.h>
#include "operator.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

namespace  vaultdb {
    template<typename  B>
    class Join : public Operator<B> {



    public:
        Join(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
        Join(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs,  Expression<B> * predicate, const SortDefinition & sort = SortDefinition());
        ~Join() { if(predicate_ != nullptr) delete predicate_; }

        // if B write is true, then write to the left side of an output tuple with src_tuple
        static void write_left(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_left(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);
        static void write_left(PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_left(SecureTuple & dst_tuple, const SecureTuple & src_tuple);

        // if B write is true, then write to the right side of an output tuple with src_tuple
        static void write_right(const bool & write, PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_right(const emp::Bit & write, SecureTuple & dst_tuple, const SecureTuple & src_tuple);
        static void write_right(PlainTuple & dst_tuple, const PlainTuple & src_tuple);
        static void write_right(SecureTuple & dst_tuple, const SecureTuple & src_tuple);

        // if B write is true, then write to the right side of an output tuple with src_tuple
        static void write_left(const B & write, QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);
        static void write_left(QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);

        static void write_right(const B & write, QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);
        static void write_right(QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);



        static QuerySchema concatenateSchemas(const QuerySchema &lhs_schema, const QuerySchema &rhs_schema, const bool &append_bool = false);

    protected:


        string getParameters() const override;


        // predicate function needs aware of encrypted or plaintext state of its inputs
        // B = BoolField || SecureBoolField
        Expression<B>  *predicate_;


    };




}



#endif //_JOIN_H
