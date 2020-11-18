#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include <cstdint>
#include <query_table/query_tuple.h>
#include <limits.h>
#include <cfloat>

using namespace vaultdb::types;

namespace vaultdb {
    class GroupByAggregateImpl {
    public:
        explicit GroupByAggregateImpl(const int32_t & ordinal, const types::TypeId & aggType) :
                                                            aggregateOrdinal(ordinal), aggregateType(aggType),
                                                            zero(TypeUtilities::getZero(aggregateType)), one(TypeUtilities::getOne(aggregateType)){};
        virtual void initialize(const QueryTuple & tuple, const types::Value & isDummy) = 0; // run this when we start a new group-by bin
        virtual void accumulate(const QueryTuple & tuple, const types::Value & isDummy) = 0;
        virtual types::Value getResult() = 0;
        types::TypeId getType() { return aggregateType; }

        // specialized in plain or  version of aggregator
        virtual types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) = 0;
        virtual void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) = 0;
    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregateOrdinal;
        types::TypeId aggregateType;
        types::Value zero;
        types::Value one;

    };


    class PlainGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit PlainGroupByAggregateImpl(const int32_t & ordinal, const types::TypeId & aggType) : GroupByAggregateImpl(ordinal, aggType) {};

         types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin) override;
         void updateGroupByBinBoundary(const types::Value & isNewBin, types::Value & nonDummyBinFlag) override;


    };

    class GroupByCountImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const types::TypeId & aggType) : PlainGroupByAggregateImpl(ordinal, aggType), runningCount(types::TypeId::INTEGER64, 0L) {};
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~GroupByCountImpl() = default;

    private:
        types::Value runningCount;

    };

    class GroupBySumImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const types::TypeId & aggType) : PlainGroupByAggregateImpl(ordinal, aggType) {
            if(aggregateType == types::TypeId::INTEGER32) {
                aggregateType = types::TypeId::INTEGER64; // accommodate psql handling of sum for validation
                zero = TypeUtilities::getZero(aggregateType);
                one = TypeUtilities::getOne(aggregateType);
            }
        };
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~GroupBySumImpl() = default;

    private:
        types::Value runningSum;

    };

    class GroupByAvgImpl : public  PlainGroupByAggregateImpl {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const types::TypeId & aggType);
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~GroupByAvgImpl() = default;

    private:
        types::Value runningSum;
        types::Value runningCount;

    };

    class GroupByMinImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupByMinImpl(const int32_t & ordinal, const types::TypeId & aggType);
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~GroupByMinImpl() = default;

    private:
        Value runningMin;
        void resetRunningMin();


    };

    class GroupByMaxImpl : public  PlainGroupByAggregateImpl {
    public:
         GroupByMaxImpl(const int32_t & ordinal, const types::TypeId & aggType);;
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        ~GroupByMaxImpl() = default;

    private:
        types::Value runningMax;
        void resetRunningMax();


    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
