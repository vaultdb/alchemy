#ifndef _SCALAR_AGGREGATE_IMPL_H
#define _SCALAR_AGGREGATE_IMPL_H


#include <query_table/query_tuple.h>

namespace vaultdb {
    class ScalarAggregateImpl {
    public:
        ScalarAggregateImpl(const uint32_t & ordinal) : aggregateOrdinal(ordinal)  {};
        virtual void initialize(const QueryTuple & tuple) = 0; // needs to run this once with first tuple to set up state
        virtual void accumulate(const QueryTuple & tuple) = 0;
        virtual types::Value getResult() = 0;
        virtual types::TypeId getType() = 0;

    protected:

        uint32_t aggregateOrdinal;
        bool initialized = false;

    };


    class ScalarCount : public ScalarAggregateImpl {
    public:
        ScalarCount(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        int64_t runningCount;

    };


    class ScalarSum : public ScalarAggregateImpl {
    public:
        ScalarSum(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        types::Value runningSum;
        types::Value zero;

    };


    // TODO: implement avg, min, max
    class ScalarMin : public ScalarAggregateImpl {
    public:
      ScalarMin(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
      void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
      void accumulate(const QueryTuple & tuple) override;
      types::Value getResult() override;
      types::TypeId getType() override;

    private:
      types::Value runningMin;
      types::Value zero;

    };

    class ScalarMax : public ScalarAggregateImpl {
      public:
        ScalarMax(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

      private:
        types::Value runningMax;
        types::Value zero;

    };


   class ScalarAverage : public ScalarAggregateImpl {
    public:
        ScalarAverage(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        types::Value getResult() override;
        types::TypeId getType() override;

    private:
        types::Value runningSum;
        types::Value runningCount;
        types::Value zero;
        types::Value one;
        types::Value average;

    };
}

#endif
