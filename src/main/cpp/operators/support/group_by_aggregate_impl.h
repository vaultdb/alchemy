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
                aggregateOrdinal(ordinal), aggregateType(aggType),
                zero(FieldFactory<B>::getZero(aggregateType)), one(FieldFactory<B>::getOne(aggregateType)){};

        virtual ~GroupByAggregateImpl() = default;
        virtual void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) = 0; // run this when we start a new group-by bin
        virtual void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) = 0;
        virtual Field<B> getResult() = 0;
        FieldType getType() { return aggregateType; }

        // specialized in plain or secure version of aggregator
         B getDummyTag(const B & isLastEntry, const B & nonDummyBin);
        void updateGroupByBinBoundary(const B & isNewBin, B & nonDummyBinFlag);
    protected:
        void resetType(const FieldType & fieldType) {

        }

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregateOrdinal;
        FieldType aggregateType;
        Field<B> zero;
        Field<B> one;

    };


   /* template<typename B>
    class PlainGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit PlainGroupByAggregateImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl(ordinal, aggType) {};
        virtual ~PlainGroupByAggregateImpl() = default;
        Field<B> getDummyTag(const Field<B> & isLastEntry, const Field<B> & nonDummyBin) override;
        void updateGroupByBinBoundary(const Field<B> & isNewBin, Field<B> & nonDummyBinFlag) override;


    };*/

   template<typename B>
   class GroupByCountImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl<B>(ordinal, aggType),
                runningCount(FieldType::INTEGER64, 0L) {};
        void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        Field<B> getResult() override;
        ~GroupByCountImpl() = default;

    private:
        Field<B> runningCount;

    };

    template<typename B>
    class GroupBySumImpl : public  GroupByAggregateImpl<B> {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl<B>(ordinal, aggType) {
            if(aggType == FieldType::INT) {
                // TODO: move this to get result
                GroupByAggregateImpl<B>::aggregateType = FieldType::LONG; // accommodate psql handling of sum for validation
                GroupByAggregateImpl<B>::zero = FieldFactory<B>::getZero(GroupByAggregateImpl<B>::aggregateType);
                GroupByAggregateImpl<B>::one = FieldFactory<B>::getOne(GroupByAggregateImpl<B>::aggregateType);

            }
        };
        void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        Field<B> getResult() override;
        ~GroupBySumImpl() = default;

    private:
        Field<B> runningSum;

    };

    template<typename B>
    class GroupByAvgImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
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
        void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        Field<B> getResult() override;
        ~GroupByMinImpl() = default;

    private:
        Field<B> runningMin;


    };

    template<typename B>
    class GroupByMaxImpl : public  GroupByAggregateImpl<B> {
    public:
        GroupByMaxImpl(const int32_t & ordinal, const FieldType & aggType);;
        void initialize(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        void accumulate(const QueryTuple<B> & tuple, const Field<B> & isDummy) override;
        Field<B> getResult() override;
        ~GroupByMaxImpl() = default;

    private:
        Field<B> runningMax;


    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
