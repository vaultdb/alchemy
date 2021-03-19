#ifndef _SECURE_GROUP_BY_AGGREGATE_IMPL_H
#define _SECURE_GROUP_BY_AGGREGATE_IMPL_H

#include <operators/support/group_by_aggregate_impl.h>


namespace vaultdb {
    class SecureGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit SecureGroupByAggregateImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl(
                ordinal) {};
        virtual ~SecureGroupByAggregateImpl() {}
        Field * getDummyTag(const Field * isLastEntry, const Field * nonDummyBin) override;
        void updateGroupByBinBoundary(const Field * isNewBin, Field * nonDummyBinFlag) override;

    };


    class SecureGroupByCountImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupByCountImpl(const int32_t & ordinal, const FieldType & aggType) : SecureGroupByAggregateImpl(ordinal, aggType),
            runningCount(emp::Integer(64, 0L, emp::PUBLIC)) {}
        void initialize(const QueryTuple & tuple, const Field * isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field * isDummy) override;
        const Field * getResult() const override;
        ~SecureGroupByCountImpl() = default;

    private:
        emp::Integer runningCount;


    };

    class SecureGroupBySumImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupBySumImpl(const int32_t & ordinal, const FieldType & aggType) : SecureGroupByAggregateImpl(ordinal, aggType)  {
            if(aggregateType == FieldType::SECURE_INT32) {
                aggregateType = FieldType::SECURE_INT64; // accommodate psql handling of sum for validation
                zero = FieldFactory::getZero(aggregateType);
                one = FieldFactory::getOne(aggregateType);
            }
            runningSum = zero;
        };
        void initialize(const QueryTuple & tuple, const Field * isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field * isDummy) override;
        const Field * getResult() const override;
        ~SecureGroupBySumImpl() { delete runningSum; }

    private:
        Field * runningSum;

    };


    // TODO: convert this to emp::Float instead of Field* for member variables
    //  repeat the process for other 3 avg impls
    class SecureGroupByAvgImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupByAvgImpl(const int32_t & ordinal, const FieldType & aggType);;
        void initialize(const QueryTuple & tuple, const Field * isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field * isDummy) override;
        const Field * getResult() const override;
        ~SecureGroupByAvgImpl()  = default;

    private:
        emp::Float runningSum;
        emp::Float runningCount;
        emp::Float zeroFloat;
        emp::Float oneFloat;

    };

    class SecureGroupByMinImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupByMinImpl(const int32_t & ordinal, const FieldType & aggType) : SecureGroupByAggregateImpl(ordinal, aggType), runningMin(
                getMaxValue()) {};
        void initialize(const QueryTuple & tuple, const Field * isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field * isDummy) override;
        const Field * getResult() const override;
        ~SecureGroupByMinImpl()  { delete runningMin; }

    private:
        Field * runningMin;
        Field *getMaxValue() const;

    };

    class SecureGroupByMaxImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupByMaxImpl(const int32_t & ordinal, const FieldType & aggType) : SecureGroupByAggregateImpl(ordinal, aggType), runningMax(getMinValue()) {};
        void initialize(const QueryTuple & tuple, const Field * isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field * isDummy) override;
        const Field * getResult() const override;
        ~SecureGroupByMaxImpl() { delete runningMax; }

    private:
        Field * runningMax;
        Field *getMinValue() const;

    };
}

#endif //_SECURE_GROUP_BY_AGGREGATE_IMPL_H
