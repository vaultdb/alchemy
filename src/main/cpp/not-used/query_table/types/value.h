#pragma once
#include "common/defs.h"
#include "emp-tool/emp-tool.h"
#include "type_id.h"
#include <boost/variant.hpp>
#include <cstdint>
#include <memory>
#include <query_table/query_field_desc.h>
class variant;

using namespace emp;

namespace vaultdb::types {
class Value {

public:
  Value();

  Value(const int32_t & val);
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
  void setValue(const Bit &val);
  void setValue(TypeId type, const Integer &val);
  void setValue(const float &val);
  void setValue(const string &aString);
  void setValue(const Float &val);

  ~Value();

  TypeId getType() const;
  int64_t getInt64() const;
  int32_t getInt32() const;
  bool getBool() const;
  Integer getEmpInt() const;
  Bit getEmpBit() const;
    std::string getVarchar() const;
    float getFloat32() const;


    string toString() const;
    Value& operator=(const Value& other); // copy assign operator overload




  void serialize(int8_t *dst) const;

  Float getEmpFloat32() const;

  Value reveal(const int &empParty = PUBLIC) const;

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

  static void compareAndSwap(Value &lhs, Value &rhs, const Bit &cmp);
  static Value obliviousIf(const Bit &cmp, Value &lhs, Value &rhs);
  // cmp is always an emp::Bit, this is just syntactic sugar
  static Value obliviousIf(const Value &cmp, Value &lhs, Value &rhs);
  static Value toFloat(const Value & src);

    void setType(TypeId type);

    static Value deserialize(QueryFieldDesc desc, int8_t *cursor);
    static Value deserialize(QueryFieldDesc desc, Bit *cursor);


protected:
  TypeId type_ = TypeId::BOOLEAN; // default setting




protected:
  struct ValueStruct {
    boost::variant<bool, int32_t, int64_t, float_t, double_t, string>
        unencrypted_val;

    Bit emp_bit_;
    Integer emp_integer_;
    Float emp_float32_;

  } value_{false, Bit(false), Integer(32,0), Float(0.0)};
  // false, Bit(false, PUBLIC), Integer(1, 0, PUBLIC),
  // Float(24, 9, 0, PUBLIC), Float32(0.0, PUBLIC)
  // };
};
     std::ostream &operator<<(std::ostream &os, const Value &value);

} // namespace vaultdb::types
