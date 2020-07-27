#include <iso646.h>
#include <vaultdb.h>

#include "value.h"
#include "util/type_utilities.h"

namespace vaultdb::types {

Value::Value() {}
Value::Value(const Value &val) {
    initialize(val);
}

Value::Value(int64_t val) {
    setValue(val);
}

Value::Value(int32_t val) {
    setValue(val);
}
Value::Value(bool val) {
    setValue(val);
}

Value::Value(emp::Bit val) {
    setValue(val);
}


Value::Value(TypeId type, const emp::Integer val) {
    setValue(type, val);
}

Value::Value(double val) {
    setValue(val);
}

Value::Value(float val) {
    setValue(val);
}

Value::Value(const string &val) {
        setValue(val);
    }


TypeId Value::getType() const { return Value::type_; }

int32_t Value::getInt32() const { return value_.unencrypted_val.int32_val; }
bool Value::getBool() const { return value_.unencrypted_val.bool_val; }
int64_t Value::getInt64() const { return value_.unencrypted_val.int64_val; }
emp::Integer *Value::getEmpInt() const { return value_.emp_integer_; }
emp::Bit *Value::getEmpBit() const { return value_.emp_bit_; }
float Value::getFloat32() const { return value_.unencrypted_val.float_val; }
double Value::getFloat64() const { return value_.unencrypted_val.double_val; }


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

void Value::setValue(const Value *v) {
  switch (v->type_) {

  case TypeId::INVALID:
    break;
  case TypeId::BOOLEAN:
      setValue(v->value_.unencrypted_val.bool_val);
    break;
  case TypeId::INTEGER32:
      setValue(v->value_.unencrypted_val.int32_val);
    break;
  case TypeId::TIMESTAMP: // store epoch as int64_t
  case TypeId::INTEGER64:
      setValue(v->value_.unencrypted_val.int64_val);
    break;
      case TypeId::TIME: // store time and date as int32_t
      case TypeId::DATE:
      case TypeId::NUMERIC:
  case TypeId::FLOAT32:
      setValue(v->value_.unencrypted_val.float_val);
    break;
  case TypeId::FLOAT64:
      setValue(v->value_.unencrypted_val.double_val);
    break;


  case TypeId::VARCHAR: {
      std::string valueStr = v->getVarchar();
      std::cout << "Parsed string 2: " << valueStr << std::endl;
      setValue(valueStr);
      break;
  }
  case TypeId::ENCRYPTED_INTEGER32:
  case TypeId::ENCRYPTED_INTEGER64:
      setValue(v->type_, *v->value_.emp_integer_);
    break;
  case TypeId::ENCRYPTED_BOOLEAN:
      setValue(*v->value_.emp_bit_);
    break;
  case TypeId::VAULT_DOUBLE:
  case TypeId::ENCRYPTED_FLOAT32:
    break;
  }
}
void Value::setValue(int32_t val) {
  type_ = TypeId::INTEGER32;
  is_encrypted_ = false;
  len_ = sizeof(int32_t);
  value_.unencrypted_val.int32_val = val;
}

void Value::setValue(int64_t val) {
  type_ = TypeId::INTEGER64;
  is_encrypted_ = false;
  len_ = sizeof(int64_t);
  value_.unencrypted_val.int64_val = val;
}

void Value::setValue(bool val) {
  type_ = TypeId::BOOLEAN;
  is_encrypted_ = false;
  len_ = sizeof(uint8_t);
  value_.unencrypted_val.bool_val = val;
}

void Value::setValue(double val) {
  type_ = TypeId::FLOAT64;
  is_encrypted_ = false;
  len_ = sizeof(double);
  value_.unencrypted_val.double_val = val;
}

void Value::setValue(float val) {
    type_ = TypeId::FLOAT32;
    is_encrypted_ = false;
    len_ = sizeof(float);
    value_.unencrypted_val.float_val = val;
}

void Value::setValue(std::string aString) {
        type_ = TypeId::VARCHAR;
        is_encrypted_ = false;
        len_ = aString.size();
        value_.unencrypted_val = aString; // = ValueStruct(std::make_unique<std::string>(aString));
}


    void Value::setValue(emp::Bit val) {
        type_ = TypeId::ENCRYPTED_BOOLEAN;
        is_encrypted_ = true;
        len_ = 1;
        value_.emp_bit_ = new emp::Bit(val.bit);
    }
    void Value::setValue(TypeId type, emp::Integer val) {
        type_ = type;
        is_encrypted_ = true;
        len_ = val.length;
        value_.emp_integer_ = new emp::Integer(val);
    }




    std::ostream& operator<<(std::ostream &strm, types::Value aValue) {
        string valueStr = aValue.getValueString();
        return strm << valueStr;

    }


     string Value::getValueString() const {
        switch (type_) {

            case TypeId::BOOLEAN:
                return  value_.unencrypted_val.bool_val ? "true" : "false";
                break;
            case TypeId::INTEGER32:
                 return std::to_string(value_.unencrypted_val.int32_val);
            case TypeId::INTEGER64:
                return std::to_string(value_.unencrypted_val.int64_val);
            case TypeId::NUMERIC:
            case TypeId::FLOAT32:
                return std::to_string(value_.unencrypted_val.float_val);
            case TypeId::FLOAT64:
                return std::to_string(value_.unencrypted_val.double_val);
            case TypeId::VAULT_DOUBLE:
                return std::to_string(value_.unencrypted_val.double_val);
                break;
            case TypeId::TIMESTAMP:
                return "timestamp";
            case TypeId::TIME:
                break;
            case TypeId::DATE:
                return std::to_string(value_.unencrypted_val.date_val);
            case TypeId::VARCHAR:
                return  getVarchar();
            case TypeId::ENCRYPTED_INTEGER32: {
                //int32_t decrypted = .getEmpInt()->reveal<int32_t>((int) EmpParty::PUBLIC);
                //return std::to_string(decrypted);
                return std::string("SECRET INT32");
            }
            case TypeId::ENCRYPTED_INTEGER64:
                {
                    //int64_t decrypted = getEmpInt()->reveal<int64_t>((int) EmpParty::PUBLIC);
                    //return std::to_string(decrypted);
                    return std::string("SECRET INT64");

                }
            case TypeId::ENCRYPTED_BOOLEAN: {
                //bool decrypted = getEmpBit()->reveal<bool>((int) EmpParty::PUBLIC); // returns a bool for both XOR and PUBLIC
                //return  decrypted ? "true" : "false";
                return std::string("SECRET BOOL");

            }
            case TypeId::ENCRYPTED_FLOAT32: {
                return "Not yet implemented";

            }
            case TypeId::INVALID:
                return "invalid";

        }
    }


    std::string Value::getVarchar() const {
        return  *(value_.unencrypted_val.varchar_val.get());
    }

    void Value::setType(TypeId type) {
        type_ = type;
    }


    void Value::initialize(const Value &val) {
            switch (val.type_) {

                case TypeId::BOOLEAN:
                    setValue(val.value_.unencrypted_val.bool_val);
                    break;
                case TypeId::INTEGER32:
                case TypeId::TIME:
                case TypeId::DATE:
                    setValue(val.value_.unencrypted_val.int32_val);
                    break;
                case TypeId::INTEGER64:
                case TypeId::TIMESTAMP: // alias for INT64
                    setValue(val.value_.unencrypted_val.int64_val);
                    break;
                case TypeId::ENCRYPTED_INTEGER32:
                case TypeId::ENCRYPTED_INTEGER64:
                    setValue(val.type_, *(val.value_.emp_integer_));
                    break;
                case TypeId::ENCRYPTED_BOOLEAN:
                    setValue(*(val.value_.emp_bit_));
                    break;
                case TypeId::VARCHAR:
                    setValue(val.getVarchar());
                    break;

                case TypeId::NUMERIC:
                case TypeId::FLOAT32:
                    setValue(val.value_.unencrypted_val.float_val);
                    break;
                case TypeId::FLOAT64:
                    setValue(val.value_.unencrypted_val.double_val);
                    break;

                case TypeId::INVALID:
                case TypeId::VAULT_DOUBLE:
                    throw;
            }
    }

    Value &Value::operator=(const Value &other) {
    if(&other == this)
        return *this;

    initialize(other);
    return *this;
    }


} // namespace vaultdb::types

