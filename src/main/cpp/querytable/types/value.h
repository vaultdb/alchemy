#include <iso646.h>
//
// Created by madhav on 1/15/20.
//

#pragma once
#include "emp-tool/emp-tool.h"
#include "type.h"
#include "type_id.h"
#include <cstdint>
#include <memory>

namespace vaultdb::types {
class Value {

public:
    Value(TypeId id, string basicString);

    friend class Type;
  friend class EncryptedIntegerType;
  friend class IntegerType;
  friend class BooleanType;
  friend class EncryptedBooleanType;
  friend class FloatType;

  Value();
  void SetValue(const Value *v);
  void SetValue(TypeId type, int32_t val);
  void SetValue(TypeId type, int64_t val);
  void SetValue(TypeId type, bool val);
  void SetValue(TypeId type, emp::Bit val);
  void SetValue(TypeId type, emp::Integer, int len);
  void SetValue(TypeId type, float val);
  void SetValue(TypeId type, double val);

  Value(TypeId type, int32_t val);
  Value(TypeId type, int64_t val);
  Value(TypeId type, bool val);
  Value(TypeId type, emp::Bit val);
  Value(TypeId type, emp::Integer, int len);
  Value(TypeId type, double val);
  Value(TypeId type, float val);

  Value(const Value &val);
  ~Value();

  TypeId GetType() const;
  int64_t GetInt64() const;
  int32_t GetInt32() const;
  [[nodiscard]] bool GetBool() const;
  [[nodiscard]] emp::Integer *GetEmpInt() const;
  [[nodiscard]] emp::Bit* GetEmpBit() const;

    string getVarchar() const { return value_.unencrypted_val.varchar_val; }
    double GetFloat() const;

    string getValueString() const;
    friend std::ostream& operator<<(std::ostream &strm, const types::Value &aValue);

protected:
  bool is_encrypted_;
  TypeId type_;
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
  struct value {
    UnencryptedVal unencrypted_val{};
    emp::Bit * emp_bit_;
    emp::Integer * emp_integer_;


      // std::unique_ptr<emp::Float32> emp_float32_;
    // std::unique_ptr<emp::Float> emp_float_;
    // std::unique_ptr<std::vector<emp::Bit>> emp_bit_array_;
  } value_{};

    void SetValue(TypeId type, char *val);
};
} // namespace vaultdb::types

