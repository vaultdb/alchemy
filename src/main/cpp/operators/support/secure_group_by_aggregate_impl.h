#ifndef _SECURE_GROUP_BY_AGGREGATE_IMPL_H
#define _SECURE_GROUP_BY_AGGREGATE_IMPL_H

#include <operators/support/group_by_aggregate_impl.h>

namespace vaultdb {
    class SecureGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit SecureGroupByAggregateImpl(const int32_t & ordinal, const types::TypeId & aggType) : GroupByAggregateImpl(ordinal, aggType) {};

        types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) override;
        void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) override;

    };


    class SecureGroupByCountImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupByCountImpl(const int32_t & ordinal, const types::TypeId & aggType) : SecureGroupByAggregateImpl(ordinal, aggType),
            runningCount(types::TypeId::ENCRYPTED_INTEGER64, emp::Integer(64, 0L, emp::PUBLIC)) {}
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~SecureGroupByCountImpl() = default;

    private:
        types::Value runningCount;


    };

    class SecureGroupBySumImpl : public  SecureGroupByAggregateImpl {
    public:
        explicit SecureGroupBySumImpl(const int32_t & ordinal, const types::TypeId & aggType) : SecureGroupByAggregateImpl(ordinal, aggType)  {
            if(aggregateType == types::TypeId::ENCRYPTED_INTEGER32) {
                aggregateType = types::TypeId::ENCRYPTED_INTEGER64; // accommodate psql handling of sum for validation
                zero = TypeUtilities::getZero(aggregateType);
                one = TypeUtilities::getOne(aggregateType);
            }
        };
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~SecureGroupBySumImpl() = default;

    private:
        types::Value runningSum;

    };

}

#endif //_SECURE_GROUP_BY_AGGREGATE_IMPL_H
