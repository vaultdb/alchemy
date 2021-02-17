#ifndef _SCALAR_AGGREGATE_IMPL_H
#define _SCALAR_AGGREGATE_IMPL_H


#include <query_table/query_tuple.h>
#include <limits.h>
#include <cfloat>

namespace vaultdb {
    class ScalarAggregateImpl {
    public:
        ScalarAggregateImpl(const uint32_t & ordinal, const types::TypeId & aggType) : aggregateOrdinal(ordinal), aggregateType(aggType),
                                                                                       zero(TypeUtilities::getZero(aggregateType)), one(TypeUtilities::getOne(aggregateType)){};
        virtual ~ScalarAggregateImpl() {}
        virtual void initialize(const QueryTuple & tuple) = 0; // needs to run this once with first tuple to set up state
        virtual void accumulate(const QueryTuple & tuple) = 0;
        virtual types::Value getResult() = 0;
        virtual types::TypeId getType() { return aggregateType; }

    protected:

        uint32_t aggregateOrdinal;
        bool initialized = false;
        types::TypeId aggregateType;
        types::Value zero;
        types::Value one;

    };


    class ScalarCount : public ScalarAggregateImpl {
    public:
        ScalarCount(const uint32_t & ordinal, const types::TypeId & aggType) :  ScalarAggregateImpl(ordinal, aggType) {}
        ~ScalarCount() {}
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        int64_t runningCount;

    };


    class ScalarSum : public ScalarAggregateImpl {
    public:
        ScalarSum(const uint32_t & ordinal, const types::TypeId & aggType) :  ScalarAggregateImpl(ordinal, aggType) {}
        ~ScalarSum() = default;
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        types::Value runningSum;

    };


    class ScalarMin : public ScalarAggregateImpl {
    public:
      ScalarMin(const uint32_t & ordinal, const types::TypeId & aggType);
      ~ScalarMin() = default;
      void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
      void accumulate(const QueryTuple & tuple) override;
      types::Value getResult() override;
      types::TypeId getType() override;

    private:
      types::Value runningMin;
      types::TypeId minType;
      void resetRunningMin();

    };

    class ScalarMax : public ScalarAggregateImpl {
      public:
        ScalarMax(const uint32_t & ordinal, const types::TypeId & aggType);
        ~ScalarMax() = default;
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

      private:
        types::Value runningMax;
        types::TypeId maxType;
        void resetRunningMax();
    };


   class ScalarAverage : public ScalarAggregateImpl {
    public:
        ScalarAverage(const uint32_t & ordinal, const types::TypeId & aggType) :  ScalarAggregateImpl(ordinal, aggType) {}
        ~ScalarAverage() = default;
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        types::Value runningSum;
        types::Value runningCount;

    };
}

#endif
