#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include <cstdint>
#include <query_table/query_tuple.h>

namespace vaultdb {
    class GroupByAggregateImpl {
    public:
        GroupByAggregateImpl(const int32_t & ordinal) : aggregateOrdinal(ordinal) {};
        virtual void initialize(const QueryTuple & tuple, const types::Value & isDummy) = 0; // run this when we start a new group-by bin
        virtual void accumulate(const QueryTuple & tuple, const types::Value & isDummy) = 0;
        virtual types::Value getResult() = 0;
        virtual types::TypeId getType() = 0;

        // specialized in plain or secure version of aggregator
        virtual types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) = 0;
        virtual void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) = 0;
    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregateOrdinal;

    };


    class PlainGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit PlainGroupByAggregateImpl(const int32_t & ordinal) : GroupByAggregateImpl(ordinal) {};

         types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) override;
         void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) override;


    };

    class GroupByCountImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal) : PlainGroupByAggregateImpl(ordinal), runningCount(0) {};
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        types::TypeId getType() override;
        ~GroupByCountImpl() = default;

    private:
        int64_t runningCount;

    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
