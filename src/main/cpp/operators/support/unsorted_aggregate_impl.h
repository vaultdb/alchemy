#ifndef _UNSORTED_AGGREGATE_IMPL_
#define  _UNSORTED_AGGREGATE_IMPL_

#include <query_table/field/field.h>
#include <operators/support/aggregate_id.h>
#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>
#include <query_table/field/field_factory.h>

namespace vaultdb {
    // generic parent class
    template<typename B>
    class UnsortedAggregateImpl {
    public:
        UnsortedAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        virtual void update(const QueryTuple<B> & input,  QueryTuple<B> & output, const B & match_found, const B & group_by_match) = 0;



        AggregateId agg_type_;
        FieldType  field_type_;
        int32_t input_ordinal_;
        int32_t output_ordinal_;

    };


    template<typename B>
    class UnsortedStatelessAggregateImpl : public UnsortedAggregateImpl<B> {
    public:
        UnsortedStatelessAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
         void update(const QueryTuple<B> & input,  QueryTuple<B> & output, const B & match_found, const B & group_by_match) override;


    };



    // maintain state for avg
    template<typename B>
    class UnsortedAvgImpl : public  UnsortedAggregateImpl<B> {
    public:
        UnsortedAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void update(const QueryTuple<B> & input,  QueryTuple<B> & output, const B & match_found, const B & group_by_match) override;

    private:
        Field<B>  running_sum_;
        Field<B>  tuple_count_;

    };


}
#endif