#ifndef _GROUP_BY_AGGREGATE_IMPL_H
#define _GROUP_BY_AGGREGATE_IMPL_H


#include "query_table/field/bool_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/float_field.h"

#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <climits>
#include <cfloat>
#include <cstdint>



namespace vaultdb {
    class GroupByAggregateImpl {
    public:
        explicit GroupByAggregateImpl(const int32_t & ordinal, const FieldType & aggType) :
                                                            aggregateOrdinal(ordinal), aggregateType(aggType),
                                                            zero(FieldFactory::getZero(aggregateType)), one(FieldFactory::getOne(aggregateType)){};

        virtual ~GroupByAggregateImpl() {}
        virtual void initialize(const QueryTuple & tuple, const Field *isDummy) = 0; // run this when we start a new group-by bin
        virtual void accumulate(const QueryTuple & tuple, const Field *isDummy) = 0;
        virtual const Field * getResult() const = 0;
        const FieldType getType() { return aggregateType; }

        // specialized in plain or  version of aggregator
        virtual const Field * getDummyTag(const Field *isLastEntry, const Field *nonDummyBin) = 0;
        virtual void updateGroupByBinBoundary(const Field *isNewBin, Field *nonDummyBinFlag) = 0;
    protected:

        // signed int because -1 denotes *, as in COUNT(*)
        int32_t aggregateOrdinal;
        FieldType aggregateType;
        Field *zero;
        Field *one;

    };


    class PlainGroupByAggregateImpl : public GroupByAggregateImpl {
    public:
        explicit PlainGroupByAggregateImpl(const int32_t & ordinal, const FieldType & aggType) : GroupByAggregateImpl(ordinal, aggType) {};
        virtual ~PlainGroupByAggregateImpl() = default;
         const Field * getDummyTag(const Field *isLastEntry, const Field *nonDummyBin) override;
         void updateGroupByBinBoundary(const Field *isNewBin, Field *nonDummyBinFlag) override;


    };

    class GroupByCountImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupByCountImpl(const int32_t & ordinal, const FieldType & aggType)
        : PlainGroupByAggregateImpl(ordinal, aggType), runningCount(new LongField((int64_t) 0L)) {};
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        const Field * getResult() const override;
        ~GroupByCountImpl() { delete runningCount; }

    private:
        Field *runningCount;

    };

    class GroupBySumImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupBySumImpl(const int32_t & ordinal, const FieldType & aggType) : PlainGroupByAggregateImpl(ordinal, aggType) {
            if(aggregateType == FieldType::INT32) {
                aggregateType = FieldType::INT64; // accommodate psql handling of sum for validation
                zero = FieldFactory::getZero(aggregateType);
                one = FieldFactory::getOne(aggregateType);
            }
        };
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        const Field * getResult() const override;
        ~GroupBySumImpl() {
            delete runningSum;
        }

    private:
        Field *runningSum;

    };

    class GroupByAvgImpl : public  PlainGroupByAggregateImpl {
    public:
        GroupByAvgImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        const Field * getResult() const override;
        ~GroupByAvgImpl() = default;

    private:
        Field *runningSum;
        Field *runningCount;

    };

    class GroupByMinImpl : public  PlainGroupByAggregateImpl {
    public:
        explicit GroupByMinImpl(const int32_t & ordinal, const FieldType & aggType);
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        const Field * getResult() const override;
        ~GroupByMinImpl() { delete runningMin; }

    private:
        Field *runningMin;
        void resetRunningMin();


    };

    class GroupByMaxImpl : public  PlainGroupByAggregateImpl {
    public:
         GroupByMaxImpl(const int32_t & ordinal, const FieldType & aggType);;
        void initialize(const QueryTuple & tuple, const Field *isDummy) override;
        void accumulate(const QueryTuple & tuple, const Field *isDummy) override;
        const Field * getResult() const override;
        ~GroupByMaxImpl() { delete runningMax; }

    private:
        Field *runningMax;
        void resetRunningMax();


    };
}
#endif //_GROUP_BY_AGGREGATE_IMPL_H
