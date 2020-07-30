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


Value::Value(emp::Float32 aFloat)  {
    setValue(aFloat);
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

    Value::Value(emp::Float aFloat) {
        setValue(aFloat);
    }

TypeId Value::getType() const { return Value::type_; }

// boost::variant<bool, int32_t, int64_t, float_t, double_t, std::string>
int32_t Value::getInt32() const {
    if(value_.unencrypted_val.which() == 1) { // check that this converts to a int32_t
        return boost::get<int32_t>(value_.unencrypted_val);
    }
}

bool Value::getBool() const
{
    if(value_.unencrypted_val.which() == 0) { // check that this converts to a int32_t
        return boost::get<bool>(value_.unencrypted_val);
    }
}

int64_t Value::getInt64() const {
    if(value_.unencrypted_val.which() == 2) { // check that this converts to a int32_t
        return boost::get<int64_t>(value_.unencrypted_val);
    }
}

    float_t Value::getFloat32() const  {
        if(value_.unencrypted_val.which() == 3) { // check that this converts to a int32_t
            return boost::get<float_t>(value_.unencrypted_val);
        }
    }


    double_t Value::getFloat64() const  {
        if(value_.unencrypted_val.which() == 4) { // check that this converts to a int32_t
            return boost::get<double_t>(value_.unencrypted_val);
        }
    }

    std::string Value::getVarchar() const  {
        if(value_.unencrypted_val.which() == 5) { // check that this converts to a int32_t
            return boost::get<std::string>(value_.unencrypted_val);
        }
    }

    emp::Integer *Value::getEmpInt() const { return  value_.emp_integer_; }
    emp::Bit *Value::getEmpBit() const { return value_.emp_bit_; }


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

void Value::setValue(const Value *val) {
     //std::cout << "Writing " << val->getValueString() << std::endl;
    switch (val->type_) {

        case TypeId::BOOLEAN:
            setValue(val->getBool());
            break;
        case TypeId::INTEGER32:
            setValue(val->getInt32());
            break;
        case TypeId::INTEGER64:
            setValue(val->getInt64());
            break;
        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
            setValue(val->type_, *(val->value_.emp_integer_));
            break;
        case TypeId::ENCRYPTED_BOOLEAN:
            setValue(*(val->value_.emp_bit_));
            break;
        case TypeId::VARCHAR:
            setValue(val->getVarchar());
            break;

        case TypeId::NUMERIC:
        case TypeId::FLOAT32:
            setValue(val->getFloat32());
            break;
        case TypeId::FLOAT64:
            setValue(val->getFloat64());
            break;

        case TypeId::INVALID:
            break;
    }
}
void Value::setValue(int32_t val) {
  type_ = TypeId::INTEGER32;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(int64_t val) {
  type_ = TypeId::INTEGER64;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(bool val) {
  type_ = TypeId::BOOLEAN;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(double val) {
  type_ = TypeId::FLOAT64;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(float val) {
    type_ = TypeId::FLOAT32;
    is_encrypted_ = false;
    value_.unencrypted_val = val;
}

void Value::setValue(std::string aString) {
        type_ = TypeId::VARCHAR;
        is_encrypted_ = false;
        value_.unencrypted_val = aString; // = ValueStruct(std::make_unique<std::string>(aString));
}


    void Value::setValue(emp::Bit val) {
        type_ = TypeId::ENCRYPTED_BOOLEAN;
        is_encrypted_ = true;
        value_.emp_bit_ = new emp::Bit(val.bit);
    }
    void Value::setValue(TypeId type, emp::Integer val) {
        type_ = type;
        is_encrypted_ = true;
        value_.emp_integer_ = new emp::Integer(val);
    }

    void Value::setValue(emp::Float32 val) {
        type_ = TypeId::ENCRYPTED_FLOAT32;
        is_encrypted_ = true;
        value_.emp_float32_ = new emp::Float32(val);
    }

    void Value::setValue(emp::Float val) {
        type_ = TypeId::ENCRYPTED_FLOAT64;
        is_encrypted_ = true;
        value_.emp_float_ = new emp::Float(val);
    }



    std::ostream& operator<<(std::ostream &strm, types::Value aValue) {
        string valueStr = aValue.getValueString();
        return strm << valueStr;

    }


     string Value::getValueString() const {
        switch (type_) {

            case TypeId::BOOLEAN:
                return  (getBool()) ? "true" : "false";
                break;
            case TypeId::INTEGER32:
                 return std::to_string(getInt32());

            case TypeId::INTEGER64:
                return std::to_string(getInt64());
            case TypeId::NUMERIC:
            case TypeId::FLOAT32:
                return std::to_string(getFloat32());
            case TypeId::FLOAT64:
                return std::to_string(getFloat64());

            case TypeId::VARCHAR:
                return  getVarchar();
            case TypeId::ENCRYPTED_INTEGER32: {
                // #ifdef NDEBUG
                return std::string("SECRET INT32");
                // #else
                //int32_t decrypted = .getEmpInt()->reveal<int32_t>((int) EmpParty::PUBLIC);
                //return std::to_string(decrypted);
                // #endif
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
            case TypeId::ENCRYPTED_FLOAT32:
            case TypeId::ENCRYPTED_FLOAT64:
                {
                return std::string("SECRET FLOAT");

            }
            case TypeId::INVALID:
                return "invalid";

        }
    }



    void Value::setType(TypeId type) {
        type_ = type;
    }


    void Value::initialize(const Value &val) {
            setValue(&val);
    }

    Value &Value::operator=(const Value &other) {
    if(&other == this)
        return *this;

    initialize(other);
    return *this;
    }

    // TODO: reverse the bit orders for this for emp
    void Value::serialize(bool *dst) const {

            size_t valSize = TypeUtilities::getTypeSize(type_);

            switch (type_) {
                case vaultdb::types::TypeId::BOOLEAN:
                    *dst = getBool();
                    break;
                case vaultdb::types::TypeId::INTEGER32: {
                    int32_t value = getInt32();
                    memcpy(dst, (bool *) &value, valSize);
                    break;
                }
                case vaultdb::types::TypeId::NUMERIC:
                case vaultdb::types::TypeId::FLOAT32: {
                    float value = getFloat32();
                    memcpy(dst, (bool *) &value, valSize);
                    break;
                }
                case vaultdb::types::TypeId::FLOAT64: {
                    double value = getFloat64();
                    memcpy(dst, (bool *) &value, valSize);
                    break;
                }
                case vaultdb::types::TypeId::INTEGER64: {
                    int64_t value = getInt64();
                    memcpy(dst, (bool *) &value, valSize);
                    break;
                }

                case vaultdb::types::TypeId::VARCHAR: {
                    std::string valueStr = getVarchar();
                    const char *value = valueStr.c_str();
                    size_t strLen = valueStr.size();
                    memcpy(dst, (bool *) value, strLen * 8);
                    break;
                }


            // only works for unencrypted values for now
            // TODO: add support for XOR table

                default: // unsupported type
                    throw;
            }


        }

    emp::Float32 *Value::getEmpFloat32() const {
        return value_.emp_float32_;
    }

    emp::Float *Value::getEmpFloat()  const {
        return value_.emp_float_;
    }
} // namespace vaultdb::types

