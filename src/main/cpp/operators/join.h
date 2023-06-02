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

        static void write_left(PlainTuple & dst, const PlainTable *src, const int & idx);
        static void write_left(SecureTuple & dst, const SecureTable *src, const int & idx);
        // if B write is true, then write to the left side of an output tuple with src_tuple
        static void write_left(const B & write, QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);
        static void write_left(QueryTable<B> *dst, const int & dst_idx, const QueryTable<B> *src, const int & src_idx);


        static void write_right(PlainTuple & dst, const PlainTable *src, const int & idx);
        static void write_right(SecureTuple & dst, const SecureTable *src, const int & idx);


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
