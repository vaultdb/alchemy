#include <iso646.h>
#pragma once
#include "common/defs.h"
#include "emp-tool/emp-tool.h"
#include "type_id.h"
#include <boost/variant.hpp>
#include <cstdint>
#include <memory>

class variant;
namespace vaultdb::types {
class Value {

public:
  Value();
  Value(const int32_t &val);

  // covers INT64, DATE
  Value(const types::TypeId &type, const int64_t &val);
  // default: int64_t --> TypeId::INTEGER64
  Value(const int64_t &val);
  Value(const bool &val);
  Value(const emp::Bit &val);
  Value(TypeId type, const emp::Integer &val);
  Value(const float &val);
  Value(const Value &val);

  Value(const std::string &val);
  Value(const emp::Float &float32);

  void setValue(const Value &v);
  // for unencrypted values, the TypeId is fixed.  This only becomes dynamic for
  // the encrypted values hence only take as input a TypeId for the latter.
  void setValue(const int32_t &val);
  void setValue(const int64_t &val);
  void setValue(const bool &val);
  void setValue(const emp::Bit &val);
  void setValue(TypeId type, const emp::Integer &val);
  void setValue(const float &val);
  void setValue(const std::string &aString);
  void setValue(const emp::Float &val);

  ~Value();

  TypeId getType() const;
  int64_t getInt64() const;
  int32_t getInt32() const;
  bool getBool() const;
  emp::Integer getEmpInt() const;
  emp::Bit getEmpBit() const;
  std::string getVarchar() const;
  float getFloat32() const;

  string getValueString() const;

  friend std::ostream &operator<<(std::ostream &os, const Value &table);

  Value &operator=(const Value &other); // copy assign operator overload

  void serialize(bool *dst) const;

  emp::Float getEmpFloat32() const;

  Value reveal(const int &empParty = emp::PUBLIC) const;

  // comparators
  Value operator>=(const Value &rhs) const;
  Value operator>(const Value &rhs) const;
  Value operator<=(const Value &rhs) const;
  Value operator<(const Value &rhs) const;
  Value operator!() const; // for use only with bool and emp::Bit
  Value operator==(const Value &rhs) const;
  Value operator!=(const Value &rhs) const;

  // boolean/bitwise ops
  Value operator^(const Value &rhs) const; // XOR
  Value operator&(const Value &rhs) const; // AND
  Value operator|(const Value &rhs) const; // OR

  // for use only with unencrypted instances of Value
  // TODO: refactor this into encrypted value and unencrypted value, then
  // overload an equality that returns an emp::Bit for the former
  // bool operator== (Value const & rhs);
  // bool operator!= (const Value & rhs);

  // arithmetic expressions
  Value operator+(const Value &rhs) const;
  Value operator-(const Value &rhs) const;
  Value operator*(const Value &rhs) const;
  Value operator/(const Value &rhs) const;

  static void compareAndSwap(Value &lhs, Value &rhs, const emp::Bit &cmp);
  static Value obliviousIf(const emp::Bit &cmp, Value &lhs, Value &rhs);

protected:
  bool is_encrypted_;
  TypeId type_;

public:
  void setType(TypeId type);

protected:
  struct ValueStruct {
    boost::variant<bool, int32_t, int64_t, float_t, double_t, std::string>
        unencrypted_val;
    std::shared_ptr<emp::Bit> emp_bit_;
    std::shared_ptr<emp::Integer> emp_integer_;
    std::shared_ptr<emp::Float> emp_float32_;

  } value_{false, nullptr, nullptr, nullptr};
  // false, emp::Bit(false, emp::PUBLIC), emp::Integer(1, 0, emp::PUBLIC),
  // emp::Float(24, 9, 0, emp::PUBLIC), emp::Float32(0.0, emp::PUBLIC)
  // };
};
} // namespace vaultdb::types
