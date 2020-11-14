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
}

#endif //_SECURE_GROUP_BY_AGGREGATE_IMPL_H
