#ifndef _UNSORTED_AGGREGATE_IMPL_
#define  _UNSORTED_AGGREGATE_IMPL_

#include <query_table/field/field.h>
#include <operators/support/aggregate_id.h>
#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>
#include <query_table/field/field_factory.h>
#include <query_table/query_table.h>

namespace vaultdb {
    // generic parent class
    template<typename B>
    class UnsortedAggregateImpl {
    public:
        UnsortedAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        virtual void initialize(const QueryFieldDesc &input_schema) = 0;
        virtual void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) = 0;
        virtual bool packedFields() const = 0; // is the aggregator emitting a packed field?

        AggregateId agg_type_;
        FieldType field_type_;
        int32_t input_ordinal_;
        int32_t output_ordinal_;

    };


    template<typename B>
    class UnsortedStatelessAggregateImpl : public UnsortedAggregateImpl<B> {
    public:
        UnsortedStatelessAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void initialize(const QueryFieldDesc &input_schema) override {};
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;
        bool packedFields() const override { return false; }

    };


    // maintain state for avg
    template<typename B>
    class UnsortedAvgImpl : public  UnsortedAggregateImpl<B> {
    public:
        UnsortedAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void initialize(const QueryFieldDesc &input_schema) override {};
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;
        bool packedFields() const override { return false; }
    private:
        Field<B>  running_sum_;
        Field<B>  tuple_count_;

    };

    // maintain state for avg
    template<typename B>
    class UnsortedCountImpl : public UnsortedAggregateImpl<B> {
    public:
        UnsortedCountImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void initialize(const QueryFieldDesc &input_schema) override;
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;
        bool packedFields() const override { return packed_fields_; }
    private:
        Field<B> running_count_;
        bool packed_fields_ = false;
    };

    template<typename B>
    class UnsortedMinImpl : public UnsortedAggregateImpl<B> {
    public:
        UnsortedMinImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void initialize(const QueryFieldDesc &input_schema) override;
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;
        bool packedFields() const override { return packed_fields_; }
    private:
        Field<B> running_min_;
        bool packed_fields_ = false;
    };

    template<typename B>
    class UnsortedMaxImpl : public UnsortedAggregateImpl<B> {
    public:
        UnsortedMaxImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
        void initialize(const QueryFieldDesc &input_schema) override;
        void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;
        bool packedFields() const override { return packed_fields_; }
    private:
        Field<B> running_max_;
        bool packed_fields_ = false;
    };

}
#endif