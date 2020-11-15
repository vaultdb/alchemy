#ifndef _SECURE_GROUP_BY_AGGREGATE_IMPL_H
#define _SECURE_GROUP_BY_AGGREGATE_IMPL_H

#include <operators/support/group_by_aggregate_impl.h>

namespace vaultdb {
    class SecureGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit SecureGroupByAggregateImpl(const int32_t & ordinal) : GroupByAggregateImpl(ordinal) {};

        types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) override;
        void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) override;

    };


    class SecureGroupByCountImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit SecureGroupByCountImpl(const int32_t & ordinal) : PlainGroupByAggregateImpl(ordinal), runningCount(64, 0, emp::PUBLIC), zero(64, 0, emp::PUBLIC), one(64, 1, emp::PUBLIC) {};
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        types::TypeId getType() override;
        ~SecureGroupByCountImpl() = default;

    private:
        emp::Integer runningCount;
        emp::Integer zero;
        emp::Integer one;

    };

}

#endif //_SECURE_GROUP_BY_AGGREGATE_IMPL_H
