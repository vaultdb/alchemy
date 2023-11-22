#ifndef _SCALAR_AGGREGATE_IMPL_H
#define _SCALAR_AGGREGATE_IMPL_H

#include <query_table/query_table.h>
#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <climits>
#include <cfloat>

namespace vaultdb {
    template<typename B>
    class ScalarAggregateImpl {
    public:
        ScalarAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        virtual void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst) = 0;
        virtual ~ScalarAggregateImpl() = default;
        
        AggregateId agg_type_;
        FieldType  field_type_;
        int32_t input_ordinal_;
        int32_t output_ordinal_;
        int32_t bit_packed_size_ = 64; // in bits, for count

    };


    template<typename B>
    class ScalarStatelessAggregateImpl : public ScalarAggregateImpl<B> {
    public:
        ScalarStatelessAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst) override;

    private:
        B not_initialized_ = B(true);

    };



    // maintain state for avg
    template<typename B>
    class ScalarAvgImpl : public  ScalarAggregateImpl<B> {
    public:
        ScalarAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst) override;

    private:
        Field<B>  running_sum_;
        Field<B>  tuple_count_;
        B not_initialized = B(true);
        Field<B> one_;

    };

}

#endif
