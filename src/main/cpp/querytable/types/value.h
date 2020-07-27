#include <iso646.h>
#pragma once
#include "emp-tool/emp-tool.h"
#include "type.h"
#include "type_id.h"
#include <cstdint>
#include <memory>
#include <boost/variant.hpp>


class variant;
namespace vaultdb::types {
class Value {

public:


  Value();

  void setValue(const Value *v);
  // for unencrypted values, the TypeId is fixed.  This only becomes dynamic for the encrypted values
  // hence only take as input a TypeId for the latter.
  void setValue(int32_t val);
  void setValue(int64_t val);
  void setValue(bool val);
  void setValue(emp::Bit val);
  void setValue(TypeId type, emp::Integer val);
  void setValue(float val);
  void setValue(double val);
  void setValue(std::string aString);

  Value(int32_t val);
  Value(int64_t val);
  Value(bool val);
  Value(emp::Bit val);
  Value(TypeId type, const emp::Integer val);
  Value(double val);
  Value(float val);
  Value(const Value &val);
  Value(const std::string & val);

  ~Value();

  TypeId getType() const;
  int64_t getInt64() const;
  int32_t getInt32() const;
  [[nodiscard]] bool getBool() const;
  [[nodiscard]] emp::Integer *getEmpInt() const;
  [[nodiscard]] emp::Bit* getEmpBit() const;
    std::string getVarchar() const;


    float getFloat32() const;
    double getFloat64() const;

    string getValueString() const;
    friend std::ostream& operator<<(std::ostream &strm, const types::Value &aValue);
    Value& operator=(const Value& other); // copy assign operator overload

protected:
  bool is_encrypted_;
  TypeId type_;
public:
    void setType(TypeId type);

protected:
    int64_t len_;
  union UnencryptedVal {
    uint8_t bool_val;
    int64_t int64_val;
    int32_t int32_val;
    int64_t date_val;
    float float_val;
    double double_val;
    char *varchar_val;
  };


  struct ValueStruct {
      boost::variant<bool, int32_t, int64_t, float_t, double_t, std::string> unencrypted_val;
    emp::Bit * emp_bit_;
    emp::Integer * emp_integer_;
    emp::Float *emp_float_;

  } value_{};

    void initialize(const Value &other);
};
} // namespace vaultdb::types

