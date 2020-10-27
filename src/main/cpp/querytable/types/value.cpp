#include <util/data_utilities.h>
#include <sstream>
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


Value::Value(const  emp::Float & aFloat)  {
    setValue(aFloat);
}


Value::Value(TypeId type, const emp::Integer & val) {
    setValue(type, val);
}



Value::Value(const  float & val) {
    setValue(val);
}

Value::Value(const string & val) {
        setValue(val);
    }



TypeId Value::getType() const { return Value::type_; }

// boost::variant<bool, int32_t, int64_t, float_t, double_t, std::string>
int32_t Value::getInt32() const {
    if(value_.unencrypted_val.which() == 1) { // check that this converts to a int32_t
        return boost::get<int32_t>(value_.unencrypted_val);
    }
    throw;

}

bool Value::getBool() const
{
    if(value_.unencrypted_val.which() == 0) { // check that this converts to a int32_t
        return boost::get<bool>(value_.unencrypted_val);
    }
    throw;

}

int64_t Value::getInt64() const {
    if(value_.unencrypted_val.which() == 2) { // check that this converts to a int32_t
        return boost::get<int64_t>(value_.unencrypted_val);
    }
    throw;

}

    float_t Value::getFloat32() const  {
        if(value_.unencrypted_val.which() == 3) { // check that this converts to a int32_t
            return boost::get<float_t>(value_.unencrypted_val);
        }
        throw;

    }



    std::string Value::getVarchar() const  {
        if(value_.unencrypted_val.which() == 5) { // check that this converts to a string
            return boost::get<std::string>(value_.unencrypted_val);
        }
        throw;

    }

    emp::Integer Value::getEmpInt()  const {
        assert(value_.emp_integer_.get() != nullptr); // do not return uninitialized vals


        return  *value_.emp_integer_;
    }
    emp::Bit Value::getEmpBit()  const {
        assert(value_.emp_bit_.get() != nullptr); // do not return uninitialized vals


       return *value_.emp_bit_;
    }

    emp::Float Value::getEmpFloat32() const {
        assert(value_.emp_float32_.get() != nullptr); // do not return uninitialized vals


        return  *value_.emp_float32_;
    }




    Value::~Value() {
        switch(getType()) {
            case TypeId::ENCRYPTED_INTEGER32:
            case TypeId::ENCRYPTED_INTEGER64:
            case TypeId::ENCRYPTED_VARCHAR:
                value_.emp_integer_.reset();
            case TypeId::ENCRYPTED_FLOAT32:
                value_.emp_float32_.reset();
            case TypeId::ENCRYPTED_BOOLEAN:
                value_.emp_bit_.reset();
            default:
                return; // eliminate warnings
        }

    }


void Value::setValue(const Value  & val) {
    switch (val.type_) {

        case TypeId::BOOLEAN:
            setValue(val.getBool());
            break;
        case TypeId::INTEGER32:
            setValue(val.getInt32());
            break;
        case TypeId::DATE:
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
            emp::Float floatVal = val.getEmpFloat32();
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



void Value::setValue(const float & val) {
    type_ = TypeId::FLOAT32;
    is_encrypted_ = false;
    value_.unencrypted_val = val;
}

void Value::setValue(const std::string & aString) {
        type_ = TypeId::VARCHAR;
        is_encrypted_ = false;
        value_.unencrypted_val = aString;
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

    void Value::setValue(const emp::Float & val) {
        type_ = TypeId::ENCRYPTED_FLOAT32;
        is_encrypted_ = true;
        value_.emp_float32_ = std::shared_ptr<emp::Float>(new emp::Float(val));
    }





    std::ostream &types::operator<<(std::ostream &os, const Value &aValue) {
        string valueStr = aValue.toString();
        return os << valueStr;
    }

     string Value::toString() const {

        switch (type_) {

            case TypeId::BOOLEAN:
                if(getBool())
                    return std::string("true");
                else
                    return std::string("false");
            case TypeId::INTEGER32:
                 return std::to_string(getInt32());

            case TypeId::DATE:
            case TypeId::INTEGER64:
                return std::to_string(getInt64());
            case TypeId::NUMERIC:
            case TypeId::FLOAT32: {
                float value = getFloat32();
                std::stringstream stream;
                //stream << std::fixed << std::setprecision(1) << value;
                stream << value;
                return stream.str();
            }

            case TypeId::VARCHAR:
                return  "'" + getVarchar() + "'";
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

                case vaultdb::types::TypeId::DATE:
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
            case types::TypeId::NUMERIC:
            case types::TypeId::VARCHAR:
            case types::TypeId::DATE:
                return Value(*this); // copy the public field, no need to reveal
            case types::TypeId::ENCRYPTED_BOOLEAN: {
                bool decrypted = this->getEmpBit().reveal<bool>(empParty); // returns a bool for both XOR and PUBLIC
                return Value(decrypted);
            }
            case types::TypeId::ENCRYPTED_INTEGER32: {
                int32_t dst = this->getEmpInt().reveal<int32_t>(empParty);
                return types::Value(dst);

                /*std::bitset<32> bs;
                bs.reset();
                bool b[32] = { false };
                emp::ProtocolExecution::prot_exec->reveal(b, empParty, (emp::block *)anInt.bits.data(), 32);
                for (int i = 0; i < 32; ++i)
                    bs.set(i, b[i]);
                int32_t intValue = bs.to_ulong();
                return types::Value(intValue);*/

            }

            case types::TypeId::ENCRYPTED_INTEGER64: {
                int64_t dst = this->getEmpInt().reveal<int64_t>(empParty);
                return types::Value(dst);
            }
            case types::TypeId::ENCRYPTED_FLOAT32: {
                float_t dst = this->getEmpFloat32().reveal<double>(empParty);
                return types::Value(dst);
            }


            case types::TypeId::ENCRYPTED_VARCHAR: {

                emp::Integer encryptedString = this->getEmpInt();
                long bitCount = encryptedString.size();
                long byteCount = bitCount / 8;


                bool *bools = new bool[bitCount];
                //emp::ProtocolExecution::prot_exec->reveal(bools,  empParty, (emp::block *) encryptedString.bits.data(), bitCount);
                std::string bitString = this->getEmpInt().reveal<std::string>(emp::PUBLIC);
                std::string::iterator strPos = bitString.begin();
                for(int i =  0; i < bitCount; ++i) {
                    bools[i] = (*strPos == '1');
                    ++strPos;
                }

                char *decodedBytes = (char *) DataUtilities::boolsToBytes(bools, bitCount);
                char *intermediary = new char[byteCount + 1];
                memcpy(intermediary, decodedBytes, byteCount);
                intermediary[byteCount] = '\0';



                std::string dst(intermediary);
                Value dstValue(dst);

                delete [] decodedBytes;
                delete [] bools;
                delete [] intermediary;

                return dstValue;
            }

            default: // all other types are unencrypted, so just copy out the value
                if(!is_encrypted_) {
                    return types::Value(*this);
                }
                else {
                    throw;
                }
        };
}





} // namespace vaultdb::types

