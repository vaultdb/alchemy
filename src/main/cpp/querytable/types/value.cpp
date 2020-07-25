#include <iso646.h>
#include <vaultdb.h>

#include "value.h"
#include "util/type_utilities.h"

namespace vaultdb::types {

Value::Value() {}
Value::Value(const Value &val)
    : len_(val.len_), type_(val.type_), is_encrypted_(val.is_encrypted_) {
  switch (val.type_) {

  case TypeId::BOOLEAN:
    Value(val.type_, val.value_.unencrypted_val.bool_val);
    break;
  case TypeId::INTEGER32:
    Value(val.type_, val.value_.unencrypted_val.int32_val);
    break;
  case TypeId::INTEGER64:
    Value(val.type_, val.value_.unencrypted_val.int64_val);
    break;
  case TypeId::ENCRYPTED_INTEGER32:
  case TypeId::ENCRYPTED_INTEGER64:
    Value(val.type_, *(val.value_.emp_integer_), val.len_);
    break;
  case TypeId::ENCRYPTED_BOOLEAN:
    Value(val.type_, *(val.value_.emp_bit_));
    break;
  case TypeId::INVALID:
  case TypeId::FLOAT32:
  case TypeId::FLOAT64:
  case TypeId::VAULT_DOUBLE:
  case TypeId::NUMERIC:
  case TypeId::TIMESTAMP:
  case TypeId::TIME:
  case TypeId::DATE:
  case TypeId::VARCHAR:
    throw;
  }
}

Value::Value(TypeId type, int64_t val)
    : type_(type), len_(sizeof(int64_t)), is_encrypted_(false) {
  value_.unencrypted_val.int64_val = val;
}

Value::Value(TypeId type, int32_t val)
    : type_(type), len_(sizeof(int32_t)), is_encrypted_(false) {
  value_.unencrypted_val.int32_val = val;
}
Value::Value(TypeId type, bool val)
    : type_(type), len_(sizeof(bool)), is_encrypted_(false) {
  value_.unencrypted_val.bool_val = val;
}

Value::Value(TypeId type, emp::Bit val)
    : type_(type), len_(sizeof(bool)), is_encrypted_(true) {
  value_.emp_bit_ = new emp::Bit(val.bit);
}

Value::Value(TypeId type, const emp::Integer val, int len)
    : type_(type), len_(len), is_encrypted_(true) {
  value_.emp_integer_ = new emp::Integer(val);
}

Value::Value(TypeId type, double val)
    : type_(type), len_(sizeof(double)), is_encrypted_(false) {
  value_.unencrypted_val.double_val = val;
}

TypeId Value::GetType() const { return Value::type_; }

int32_t Value::GetInt32() const { return value_.unencrypted_val.int32_val; }
bool Value::GetBool() const { return value_.unencrypted_val.bool_val; }
int64_t Value::GetInt64() const { return value_.unencrypted_val.int64_val; }
emp::Integer *Value::GetEmpInt() const { return value_.emp_integer_; }
emp::Bit *Value::GetEmpBit() const { return value_.emp_bit_; }
double Value::GetFloat() const { return value_.unencrypted_val.double_val; }

Value::~Value() {
  /*
  switch(type_) {
  case TypeId::ENCRYPTED_BOOLEAN:
    value_.emp_bit_.reset();
    break;
  case TypeId::ENCRYPTED_INTEGER64:
  case TypeId::ENCRYPTED_INTEGER32:
    value_.emp_integer_.reset();
    break;
  }
   */
}

void Value::SetValue(const Value *v) {
  switch (v->type_) {

  case TypeId::INVALID:
    break;
  case TypeId::BOOLEAN:
    SetValue(v->type_, v->value_.unencrypted_val.bool_val);
    break;
  case TypeId::INTEGER32:
    SetValue(v->type_, v->value_.unencrypted_val.int32_val);
    break;
  case TypeId::INTEGER64:
    SetValue(v->type_, v->value_.unencrypted_val.int64_val);
    break;
  case TypeId::FLOAT32:
    SetValue(v->type_, v->value_.unencrypted_val.double_val);
    break;
  case TypeId::FLOAT64:
    break;
  case TypeId::VAULT_DOUBLE:
    break;
  case TypeId::NUMERIC:
    break;
  case TypeId::TIMESTAMP:
    break;
  case TypeId::TIME:
    break;
  case TypeId::DATE:
    break;
  case TypeId::VARCHAR:
      SetValue(v->type_,  v->value_.unencrypted_val.varchar_val);
    break;
  case TypeId::ENCRYPTED_INTEGER32:
  case TypeId::ENCRYPTED_INTEGER64:
    SetValue(v->type_, *v->value_.emp_integer_, v->len_);
    break;
  case TypeId::ENCRYPTED_BOOLEAN:
    SetValue(v->type_, *v->value_.emp_bit_);
    break;
  case TypeId::ENCRYPTED_FLOAT32:
    break;
  }
}
void Value::SetValue(TypeId type, int32_t val) {
  type_ = type;
  is_encrypted_ = false;
  len_ = sizeof(int32_t);
  value_.unencrypted_val.int32_val = val;
}
void Value::SetValue(TypeId type, int64_t val) {
  type_ = type;
  is_encrypted_ = false;
  len_ = sizeof(int64_t);
  value_.unencrypted_val.int64_val = val;
}
void Value::SetValue(TypeId type, bool val) {
  type_ = type;
  is_encrypted_ = false;
  len_ = sizeof(uint8_t);
  value_.unencrypted_val.bool_val = val;
}
void Value::SetValue(TypeId type, emp::Bit val) {
  type_ = type;
  is_encrypted_ = true;
  len_ = 1;
  value_.emp_bit_ = new emp::Bit(val.bit);
}
void Value::SetValue(TypeId type, emp::Integer val, int len) {
  type_ = type;
  is_encrypted_ = true;
  len_ = len;
  value_.emp_integer_ = new emp::Integer(val);
}
void Value::SetValue(TypeId type, double val) {
  type_ = type;
  is_encrypted_ = false;
  len_ = sizeof(double);
  value_.unencrypted_val.double_val = val;
}

Value::Value(TypeId id, string basicString) {
        type_ = id;
        is_encrypted_ = false;
        len_ = sizeof(basicString.c_str());
        char *varchar = new char[basicString.size()];
        memcpy(varchar, basicString.c_str(), basicString.size());
        value_.unencrypted_val.varchar_val = varchar;

    }

    std::ostream& operator<<(std::ostream &strm, const Value &aValue) {
        string typeStr = TypeUtilities::getTypeIdString(aValue.GetType());
        string valueStr = Value::getValueString(aValue);

        return strm << "(" << typeStr << "," <<  valueStr << ")";
    }


     string Value::getValueString(Value v) {
        switch (v.type_) {

            case TypeId::BOOLEAN:
                return  v.value_.unencrypted_val.bool_val ? "true" : "false";
                break;
            case TypeId::INTEGER32:
                 return std::to_string(v.value_.unencrypted_val.int32_val);
            case TypeId::INTEGER64:
                return std::to_string(v.value_.unencrypted_val.int64_val);
            case TypeId::NUMERIC:
            case TypeId::FLOAT32:
                return std::to_string(v.value_.unencrypted_val.double_val);
            case TypeId::FLOAT64:
                return std::to_string(v.value_.unencrypted_val.float_val);
            case TypeId::VAULT_DOUBLE:
                return std::to_string(v.value_.unencrypted_val.double_val);
                break;
            case TypeId::TIMESTAMP:
                return "timestamp";
            case TypeId::TIME:
                break;
            case TypeId::DATE:
                return std::to_string(v.value_.unencrypted_val.date_val);
            case TypeId::VARCHAR:
                break;
            case TypeId::ENCRYPTED_INTEGER32: {
                int32_t decrypted = v.GetEmpInt()->reveal<int32_t>((int) EmpParty::PUBLIC);
                return std::to_string(decrypted); }
                case TypeId::ENCRYPTED_INTEGER64:
                {
                    int64_t decrypted = v.GetEmpInt()->reveal<int64_t>((int) EmpParty::PUBLIC);
                    return std::to_string(decrypted); }
            case TypeId::ENCRYPTED_BOOLEAN: {
                bool decrypted = v.GetEmpBit()->reveal<bool>((int) EmpParty::PUBLIC); // returns a bool for both XOR and PUBLIC
                return  decrypted ? "true" : "false"; }
            case TypeId::ENCRYPTED_FLOAT32:
                return "Not yet implemented";
            case TypeId::INVALID:
                return "invalid";

        }
    }

} // namespace vaultdb::types

