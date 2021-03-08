#ifndef _SECURE_SCALAR_AGGREGATE_IMPL_H
#define _SECURE_SCALAR_AGGREGATE_IMPL_H

#include "scalar_aggregate_impl.h"
#include "query_table/field/secure_long_field.h"

namespace vaultdb {
class SecureScalarAverage : public ScalarAggregateImpl {
public:
  explicit SecureScalarAverage(const uint32_t &ordinal, const FieldType & aggType);
  ~SecureScalarAverage() = default;
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  const Field * getResult() const override;

private:
  Field * runningSum;
  Field * runningCount;
  Field * zeroFloat;
  Field * oneFloat;
};


class SecureScalarCount : public ScalarAggregateImpl {
public:
  explicit SecureScalarCount(const uint32_t &ordinal, const FieldType & aggType)
      : ScalarAggregateImpl(ordinal, FieldType::SECURE_INT64),
        runningCount( new SecureLongField(emp::Integer(64, 0L, emp::PUBLIC))){}

  ~SecureScalarCount() {}
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  const Field * getResult() const override;
  FieldType getType() override;

private:
  Field * runningCount;

};

class SecureScalarSum : public ScalarAggregateImpl {
public:
  explicit SecureScalarSum(const uint32_t &ordinal,
                           const FieldType &aggType)
      : ScalarAggregateImpl(ordinal, aggType) {
    if (aggregateType == FieldType::SECURE_INT64) {
      aggregateType =
          FieldType::SECURE_INT64; // accommodate psql handling of
                                              // sum for validation
      zero = FieldFactory::getZero(aggregateType);
    }
    runningSum = zero;
  }

  ~SecureScalarSum() {}
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  const Field * getResult() const override;
//  FieldType getType() override;

private:
  Field * runningSum;

};

class SecureScalarMin : public ScalarAggregateImpl {
public:
  explicit SecureScalarMin(const uint32_t &ordinal, const FieldType & aggType)
      : ScalarAggregateImpl(ordinal, aggType){}

  ~SecureScalarMin() {}
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  const Field * getResult() const override;
  FieldType getType() override;

private:
  Field * runningMin;
  Field * zero;
  Field * getMaxValue() const;
};


class SecureScalarMax : public ScalarAggregateImpl {
public:
  explicit SecureScalarMax(const uint32_t &ordinal, const FieldType & aggType)
      : ScalarAggregateImpl(ordinal, aggType){}
  ~SecureScalarMax() {}
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  const Field * getResult() const override;
  FieldType getType() override;

private:
  Field * runningMax;
  Field * zero;
  Field * getMinValue() const;
};
} // namespace vaultdb

#endif //_SECURE_SCALAR_AGGREGATE_IMPL_H
