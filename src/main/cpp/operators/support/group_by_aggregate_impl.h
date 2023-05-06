#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include <cstdint>
#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <limits.h>
#include <cfloat>


namespace vaultdb {
    template<typename B>
    class GroupByAggregateImpl {
    public:
        explicit GroupByAggregateImpl(const int32_t & ordinal, const FieldType & aggType) :
                aggregate_ordinal_(ordinal), aggregate_type_(aggType),
                zero_(FieldFactory<B>::getZero(aggregate_type_)), one_(FieldFactory<B>::getOne(aggregate_type_)){};

        virtual ~GroupByAggregateImpl() = default;
        virtual void initialize(const QueryTuple<B> & tuple, const B &isGroupByMatch) = 0; // run this when we start a new group-by bin
        virtual void accumulate(const QueryTuple<B> & tuple, const B &isGroupByMatch) = 0;
        virtual Field<B> getResult() = 0;
        virtual FieldType getType() const { return aggregate_type_; }

    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregate_ordinal_;
        FieldType aggregate_type_;
        Field<B> zero_;
        Field<B> one_;

    };



    template<typename B>
   class GroupByCountImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple<B> & tuple, const B & group_by_match) override;
        void accumulate(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        Field<B> getResult() override;
        FieldType getType() const override;
       ~GroupByCountImpl() = default;

    private:
        Field<B> running_count_;

    };


    template<typename B>
    class GroupBySumImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        void accumulate(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        Field<B> getResult() override;
        FieldType getType() const override;
        ~GroupBySumImpl() = default;

    private:
        Field<B> running_sum_;

    };

    template<typename B>
    class GroupByAvgImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        void accumulate(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        Field<B> getResult() override;
        ~GroupByAvgImpl() = default;

    private:
        Field<B> runningSum;
        Field<B> runningCount;

    };

    template<typename B>
    class GroupByMinImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupByMinImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        void accumulate(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        Field<B> getResult() override;
        ~GroupByMinImpl() = default;

    private:
        Field<B> runningMin;
        Field<B> resetMin;


    };

    template<typename B>
    class GroupByMaxImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByMaxImpl(const int32_t & ordinal, const FieldType & aggType);;
        void initialize(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        void accumulate(const QueryTuple<B> & tuple, const B & isGroupByMatch) override;
        Field<B> getResult() override;
        ~GroupByMaxImpl() = default;

    private:
        Field<B> runningMax;
        Field<B> resetMax;


    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
