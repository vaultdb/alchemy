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
    UnsortedAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal, const int & max_value = 0);
    virtual void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) = 0;
    virtual ~UnsortedAggregateImpl() = default;


    AggregateId agg_type_;
    FieldType  field_type_;
    int32_t input_ordinal_;
    int32_t output_ordinal_;
    Field<B> one_;
    int32_t bit_packed_size_ = 64; // in bits, for count

};


template<typename B>
class UnsortedStatelessAggregateImpl : public UnsortedAggregateImpl<B> {
public:
    UnsortedStatelessAggregateImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal, const int & max_value = 0);
    void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;


};



// maintain state for avg
template<typename B>
class UnsortedAvgImpl : public  UnsortedAggregateImpl<B> {
public:
    UnsortedAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal);
    void update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) override;

private:
    Field<B>  running_sum_;
    Field<B>  tuple_count_;
    int64_t max_value_ = 0;

};

} // namespace vaultdb
#endif