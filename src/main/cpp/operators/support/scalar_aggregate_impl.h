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
        B not_initialized = B(true);

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

    };
//    class ScalarAggregateImpl {
//    public:
//        ScalarAggregateImpl(const uint32_t & ordinal, const QueryFieldDesc &def) : ordinal_(ordinal),
//            column_def_(def), type_(def.getType()),  zero_(FieldFactory<B>::getZero(type_)),   one_(FieldFactory<B>::getOne(type_)) {}
//        virtual ~ScalarAggregateImpl() = default;
//        virtual void accumulate(const QueryTuple<B> & tuple) = 0;
//        virtual void accumulate(const QueryTable<B> *table, const int & row) = 0;
//
//        virtual  Field<B> getResult() const = 0;
//        virtual FieldType getType() const { return type_; }
//
//    protected:
//
//        uint32_t ordinal_;
//        QueryFieldDesc column_def_;
//        FieldType type_;
//        Field<B> zero_;
//        Field<B> one_;
//
//    };
//
//    template<typename B>
//    class ScalarCountImpl : public ScalarAggregateImpl<B> {
//    public:
//        ScalarCountImpl(const uint32_t & ordinal, const QueryFieldDesc & def);
//        ~ScalarCountImpl() = default;
//        void accumulate(const QueryTuple<B> & tuple) override;
//        void accumulate(const QueryTable<B> *table, const int & row) override;
//         Field<B> getResult() const override;
//
//    private:
//        Field<B> running_count_;
//
//    };
//
//
//
//    template<typename B>
//    class ScalarSumImpl : public ScalarAggregateImpl<B> {
//    public:
//        ScalarSumImpl(const uint32_t & ordinal, const QueryFieldDesc & def) : ScalarAggregateImpl<B>(ordinal, def), running_sum_(ScalarAggregateImpl<B>::zero_) {
//            ScalarAggregateImpl<B>::column_def_.setSize(TypeUtilities::getTypeSize(ScalarAggregateImpl<B>::type_));
//        }
//        ~ScalarSumImpl() = default;
//        void accumulate(const QueryTuple<B> & tuple) override;
//        void accumulate(const QueryTable<B> *table, const int & row) override;
//        Field<B> getResult() const override;
//        FieldType getType() const override;
//
//    private:
//        Field<B> running_sum_;
//
//    };
//
//
//
//    template<typename B>
//    class ScalarMinImpl : public ScalarAggregateImpl<B> {
//    public:
//      ScalarMinImpl(const uint32_t & ordinal, const QueryFieldDesc & def);
//      ~ScalarMinImpl()  = default;
//      void accumulate(const QueryTuple<B> & tuple) override;
//      void accumulate(const QueryTable<B> *table, const int & row) override;
//       Field<B> getResult() const override;
//
//    private:
//      Field<B> running_min_;
//
//    };
//
//
//
//    template<typename B>
//    class ScalarMaxImpl : public ScalarAggregateImpl<B> {
//      public:
//        ScalarMaxImpl(const uint32_t & ordinal, const QueryFieldDesc & def);
//        ~ScalarMaxImpl() = default;
//        void accumulate(const QueryTuple<B> & tuple) override;
//        void accumulate(const QueryTable<B> *table, const int & row) override;
//        Field<B> getResult() const override;
//
//      private:
//        Field<B> running_max_;
//    };
//
//
//    template<typename B>
//    class ScalarAvgImpl : public ScalarAggregateImpl<B> {
//    public:
//        // following psql convention, only outputs floats
//        ScalarAvgImpl(const uint32_t & ordinal, const QueryFieldDesc & def);
//        ~ScalarAvgImpl()  = default;
//        void accumulate(const QueryTuple<B> & tuple) override;
//        void accumulate(const QueryTable<B> *table, const int & row) override;
//
//        Field<B> getResult() const override;
//
//    private:
//        Field<B> running_sum_;
//        Field<B> running_count_;
//
//    };

}

#endif
