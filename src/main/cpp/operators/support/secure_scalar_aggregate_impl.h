#ifndef _SECURE_SCALAR_AGGREGATE_IMPL_H
#define _SECURE_SCALAR_AGGREGATE_IMPL_H

#include "scalar_aggregate_impl.h"

namespace vaultdb {
class SecureScalarAverage : public ScalarAggregateImpl {
public:
  explicit SecureScalarAverage(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){};
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningSum;
  types::Value runningCount;
  types::Value zero;
  types::Value one;
};


class SecureScalarCount : public ScalarAggregateImpl {
public:
  explicit SecureScalarCount(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){};
  void initialize(const QueryTuple &tuple)
      override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningCount;
  types::Value zero;
  types::Value one;
};


class SecureScalarSum : public ScalarAggregateImpl {
public:
  explicit SecureScalarSum(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){};
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningSum;
  types::Value zero;
};


class SecureScalarMin : public ScalarAggregateImpl {
public:
  explicit SecureScalarMin(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){};
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningMin;
  types::Value zero;
};


class SecureScalarMax : public ScalarAggregateImpl {
public:
  explicit SecureScalarMax(const uint32_t &ordinal, const types::TypeId & aggType)
      : ScalarAggregateImpl(ordinal, aggType){};
  void initialize(const QueryTuple &tuple)
  override; // needs to run this once with first tuple to set up state
  void accumulate(const QueryTuple &tuple) override;
  types::Value getResult() override;
  types::TypeId getType() override;

private:
  types::Value runningMax;
  types::Value zero;
};
} // namespace vaultdb

#endif //_SECURE_SCALAR_AGGREGATE_IMPL_H
