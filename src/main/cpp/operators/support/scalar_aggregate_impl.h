#ifndef _SCALAR_AGGREGATE_IMPL_H
#define _SCALAR_AGGREGATE_IMPL_H


#include <query_table/query_tuple.h>
#include <query_table/field/field_factory.h>
#include <climits>
#include <cfloat>

namespace vaultdb {
    // T = field type (e.g., BoolField), B = bool type (BoolField || SecureBoolField)
   // template<typename T, typename B>
    class ScalarAggregateImpl {
    public:
        ScalarAggregateImpl(const uint32_t & ordinal, const FieldType & aggType) : aggregateOrdinal(ordinal), aggregateType(aggType),
                                                                                       zero(FieldFactory::getZero(aggregateType)), one(FieldFactory::getOne(aggregateType)){};
        virtual ~ScalarAggregateImpl() {
            delete zero;
            delete one;
        }
        virtual void initialize(const QueryTuple & tuple) = 0; // needs to run this once with first tuple to set up state
        virtual void accumulate(const QueryTuple & tuple) = 0;
        virtual const Field * getResult() const = 0;
        virtual FieldType getType() { return aggregateType; }

    protected:

        uint32_t aggregateOrdinal;
        bool initialized = false;
        FieldType aggregateType;
        Field *zero;
        Field *one;

    };


    class ScalarCount : public ScalarAggregateImpl {
    public:
        ScalarCount(const uint32_t & ordinal, const FieldType & aggType) :  ScalarAggregateImpl(ordinal, aggType), runningCount(0) {}
        ~ScalarCount() = default;
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        const Field * getResult() const override;
        FieldType getType() override;

    private:
        int64_t runningCount;

    };


    class ScalarSum : public ScalarAggregateImpl {
    public:
        ScalarSum(const uint32_t & ordinal, const FieldType & aggType) :  ScalarAggregateImpl(ordinal, aggType) {}
        ~ScalarSum() {
            delete runningSum;
        }
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        const Field * getResult() const override;
        FieldType getType() override;

    private:
        Field * runningSum;

    };


    class ScalarMin : public ScalarAggregateImpl {
    public:
      ScalarMin(const uint32_t & ordinal, const FieldType & aggType);
      ~ScalarMin() {
          delete runningMin;
      }
      void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
      void accumulate(const QueryTuple & tuple) override;
      const Field * getResult() const override;
      FieldType getType() override;

    private:
      Field * runningMin;
      void resetRunningMin();

    };

    class ScalarMax : public ScalarAggregateImpl {
      public:
        ScalarMax(const uint32_t & ordinal, const FieldType & aggType);
        ~ScalarMax() {
            delete runningMax;
        }
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        const Field * getResult() const override;
        FieldType getType() override;

      private:
        Field * runningMax;
        void resetRunningMax();
    };


   class ScalarAverage : public ScalarAggregateImpl {
    public:
        ScalarAverage(const uint32_t & ordinal, const FieldType & aggType) :  ScalarAggregateImpl(ordinal, aggType) {}
        ~ScalarAverage() {
            delete runningSum;
            delete runningCount;
        }
        void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
        void accumulate(const QueryTuple & tuple) override;
        const Field * getResult() const override;
        FieldType getType() override;

    private:
        Field * runningSum;
        Field * runningCount;

    };
}

#endif
