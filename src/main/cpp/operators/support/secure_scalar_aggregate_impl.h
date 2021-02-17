#ifndef _SECURE_SCALAR_AGGREGATE_IMPL_H
#define _SECURE_SCALAR_AGGREGATE_IMPL_H

#include "scalar_aggregate_impl.h"

namespace vaultdb {
class SecureScalarAverage : public ScalarAggregateImpl {
public:
  explicit SecureScalarAverage(const uint32_t &ordinal, const types::TypeId & aggType);
  ~SecureScalarAverage() = default;
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;

private:
  types::Value runningSum;
  types::Value runningCount;
  types::Value zeroFloat;
  types::Value oneFloat;
};


class SecureScalarCount : public ScalarAggregateImpl {
public:
  explicit SecureScalarCount(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, types::TypeId::ENCRYPTED_INTEGER64),
        runningCount(types::TypeId::ENCRYPTED_INTEGER64,
                     emp::Integer(64, 0L, emp::PUBLIC)){}

  ~SecureScalarCount() {}
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningCount;

};

class SecureScalarSum : public ScalarAggregateImpl {
public:
  explicit SecureScalarSum(const uint32_t &ordinal,
                           const types::TypeId &aggType)
      : ScalarAggregateImpl(ordinal, aggType) {
    if (aggregateType == types::TypeId::ENCRYPTED_INTEGER32) {
      aggregateType =
          types::TypeId::ENCRYPTED_INTEGER64; // accommodate psql handling of
                                              // sum for validation
      zero = TypeUtilities::getZero(aggregateType);
    }
    runningSum = zero;
  }

  ~SecureScalarSum() {}
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
//  types::TypeId getType() override;

private:
  types::Value runningSum;

};

class SecureScalarMin : public ScalarAggregateImpl {
public:
  explicit SecureScalarMin(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){}

  ~SecureScalarMin() {}
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningMin;
  types::Value zero;
  types::Value getMaxValue() const;
};


class SecureScalarMax : public ScalarAggregateImpl {
public:
  explicit SecureScalarMax(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){}
  ~SecureScalarMax() {}
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningMax;
  types::Value zero;
  types::Value getMinValue() const;
};
} // namespace vaultdb

#endif //_SECURE_SCALAR_AGGREGATE_IMPL_H
