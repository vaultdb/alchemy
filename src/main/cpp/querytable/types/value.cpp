#include <iso646.h>
#include <vaultdb.h>
#include <util/data_utilities.h>

#include "value.h"
#include "util/type_utilities.h"

namespace vaultdb::types {

Value::Value() {}


Value::Value(const Value &val) {
        setValue(val);
}

Value::Value(const int64_t & val) {
    setValue(val);
}

Value::Value(const  int32_t & val) {
    setValue(val);
}
Value::Value(const bool& val) {
    setValue(val);
}

Value::Value(const emp::Bit  & val) {
    setValue(val);
}


Value::Value(const  emp::Float32 & aFloat)  {
    setValue(aFloat);
}


Value::Value(TypeId type, const emp::Integer & val) {
    setValue(type, val);
}

Value::Value(const  double & val) {
    setValue(val);
}

Value::Value(const  float & val) {
    setValue(val);
}

Value::Value(const string & val) {
        setValue(val);
    }

    Value::Value(const  emp::Float & aFloat) {
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

    emp::Integer Value::getEmpInt()  const { return  *value_.emp_integer_; }
    emp::Bit Value::getEmpBit()  const { return *value_.emp_bit_; }

    emp::Float32 Value::getEmpFloat32() const {
        return  *value_.emp_float32_;
    }

    emp::Float Value::getEmpFloat64() const {
        return  *value_.emp_float_;
    }


    Value::~Value() {

}


void Value::setValue(const Value  & val) {
    switch (val.type_) {

        case TypeId::BOOLEAN:
            setValue(val.getBool());
            break;
        case TypeId::INTEGER32:
            setValue(val.getInt32());
            break;
        case TypeId::INTEGER64:
            setValue(val.getInt64());
            break;
        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer intVal = val.getEmpInt();
            setValue(val.type_, intVal);
            break;
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            emp::Bit bitValue = val.getEmpBit();
            setValue(bitValue);
            break;
        }
        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 floatVal = val.getEmpFloat32();
            setValue(floatVal);
            break;
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float floatVal = val.getEmpFloat64();
            setValue(floatVal);
            break;
        }
        case TypeId::VARCHAR:
            setValue(val.getVarchar());
            break;

        case TypeId::NUMERIC:
        case TypeId::FLOAT32:
            setValue(val.getFloat32());
            break;
        case TypeId::FLOAT64:
            setValue(val.getFloat64());
            break;

        case TypeId::INVALID:
            break;
        default:
            throw;
    }
}



void Value::setValue(const int32_t &  val) {
  type_ = TypeId::INTEGER32;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(const int64_t & val) {
  type_ = TypeId::INTEGER64;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(const bool & val) {
  type_ = TypeId::BOOLEAN;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(const double & val) {
  type_ = TypeId::FLOAT64;
  is_encrypted_ = false;
  value_.unencrypted_val = val;
}

void Value::setValue(const float & val) {
    type_ = TypeId::FLOAT32;
    is_encrypted_ = false;
    value_.unencrypted_val = val;
}

void Value::setValue(const std::string & aString) {
        type_ = TypeId::VARCHAR;
        is_encrypted_ = false;
        value_.unencrypted_val = aString; // = ValueStruct(std::make_unique<std::string>(aString));
}


    void Value::setValue(const emp::Bit & val) {
        type_ = TypeId::ENCRYPTED_BOOLEAN;
        is_encrypted_ = true;
        value_.emp_bit_ = std::shared_ptr<emp::Bit>(new emp::Bit(val.bit));
    }
    void Value::setValue(TypeId type, const emp::Integer & val) {
        type_ = type;
        is_encrypted_ = true;
        value_.emp_integer_ = std::shared_ptr<emp::Integer>(new emp::Integer(val));
    }

    void Value::setValue(const emp::Float32 & val) {
        type_ = TypeId::ENCRYPTED_FLOAT32;
        is_encrypted_ = true;
        value_.emp_float32_ = std::shared_ptr<emp::Float32>(new emp::Float32(val));
    }

    void Value::setValue(const emp::Float & val) {
        type_ = TypeId::ENCRYPTED_FLOAT64;
        is_encrypted_ = true;
        value_.emp_float_ = std::shared_ptr<emp::Float>(new emp::Float(val));
    }




    std::ostream &vaultdb::types::operator<<(std::ostream &strm, const Value &aValue) {
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
            case TypeId::ENCRYPTED_VARCHAR:
                return std::string("SECRET VARCHAR");

            case TypeId::INVALID:
                return "invalid";

        }
    }



    void Value::setType(TypeId type) {
        type_ = type;
    }



    Value &Value::operator=(const Value &other) {
    if(&other == this)
        return *this;



    setValue(other);
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


    Value Value::reveal(const int &empParty) const {



        switch(type_) {
            case types::TypeId::INVALID:
            case types::TypeId::BOOLEAN:
            case types::TypeId::INTEGER32:
            case types::TypeId::INTEGER64:
            case types::TypeId::FLOAT32:
            case types::TypeId::FLOAT64:
            case types::TypeId::NUMERIC:
            case types::TypeId::VARCHAR:
                return Value(this); // copy the public field, no need to reveal
            case types::TypeId::ENCRYPTED_BOOLEAN: {
                bool decrypted = this->getEmpBit().reveal<bool>((int) empParty); // returns a bool for both XOR and PUBLIC
                return Value(decrypted);
            }
            case types::TypeId::ENCRYPTED_INTEGER32: {
                emp::Integer anInt = this->getEmpInt();
                int32_t dst = anInt.reveal<int32_t>((int) empParty);
                return types::Value(dst);
            }

            case types::TypeId::ENCRYPTED_INTEGER64: {
                emp::Integer anInt = this->getEmpInt();
                int64_t dst = anInt.reveal<int64_t>((int) empParty);
                return types::Value(dst);
            }
            case types::TypeId::ENCRYPTED_FLOAT32: {
                float_t dst = this->getEmpFloat32().reveal<double>((int) empParty);
                return types::Value(dst);
            }

            case types::TypeId::ENCRYPTED_FLOAT64: {
                emp::Float floatVal = this->getEmpFloat64();
                double dst = floatVal.reveal<double_t>((int) empParty);
                return types::Value(dst);

            }

            case types::TypeId::ENCRYPTED_VARCHAR: {

                emp::Integer encryptedString = this->getEmpInt();
                long bitCount = encryptedString.length;
                long byteCount = bitCount / 8;

                bool *bools = new bool[bitCount];
                emp::ProtocolExecution::prot_exec->reveal(bools, (int) empParty, (emp::block *) encryptedString.bits, bitCount);

                char *decodedBytes = (char *) DataUtilities::boolsToBytes(bools, bitCount);

                // make the char * null terminated
                char *tmp = new char[byteCount + 1];
                memcpy(tmp, decodedBytes, byteCount);
                tmp[byteCount] = '\0';
                delete [] decodedBytes;
                decodedBytes = tmp;


                std::string dst(decodedBytes);

                delete [] bools;
                delete [] decodedBytes;

                return types::Value(dst);
            }

            default: // all other types are unencrypted, so just copy out the value
                if(!is_encrypted_) {
                    return types::Value(this);
                }
                else {
                    throw;
                }
        };
}




} // namespace vaultdb::types

