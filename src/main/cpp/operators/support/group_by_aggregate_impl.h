#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include <cstdint>
#include <query_table/query_tuple.h>

namespace vaultdb {
    class GroupByAggregateImpl {
    public:
        GroupByAggregateImpl(const uint32_t & ordinal) : aggregateOrdinal(ordinal)  {};
        virtual void initialize(const QueryTuple & tuple, const types::Value & isDummy) = 0; // needs to run this once with first tuple to set up state
        virtual void accumulate(const QueryTuple & tuple, const types::Value & isDummy) = 0;

        static  types::Value getDummyTag(const types::Value & isLastEntry, const types::Value & nonDummyBin);
        virtual types::Value getResult() = 0;
        virtual types::TypeId getType() = 0;

    protected:

        uint32_t aggregateOrdinal;
        boost::variant<bool, emp::Bit> initialized;

    };


    class GroupByCountImpl : public  GroupByAggregateImpl {
    public:
        explicit GroupByCountImpl(const uint32_t & ordinal) : GroupByAggregateImpl(ordinal), runningCount(0) {};
        void initialize(const QueryTuple & tuple, const types::Value & isDummy) override;
        void accumulate(const QueryTuple & tuple, const types::Value & isDummy) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        int64_t runningCount;

    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
