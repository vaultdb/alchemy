#include <sstream>
#include "value.h"
#include "util/type_utilities.h"

namespace vaultdb::types {

Value::Value() {}


Value::Value(const Value &val) {
        setValue(val);
}


types::Value::Value(const int32_t &val) {
    setValue(val);
}


Value::Value(const int64_t & val) {
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


types::Value::Value(const types::TypeId &type, const int64_t &val) {

        type_ = type;
        value_.unencrypted_val = val;
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
        return  value_.emp_integer_;
    }

    emp::Bit Value::getEmpBit()  const {
       return value_.emp_bit_;
    }

    emp::Float Value::getEmpFloat32() const {
        return  value_.emp_float32_;
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
  value_.unencrypted_val = val;
}

void Value::setValue(const int64_t & val) {
  type_ = TypeId::INTEGER64;
  value_.unencrypted_val = val;
}

void Value::setValue(const bool & val) {
  type_ = TypeId::BOOLEAN;
  value_.unencrypted_val = val;
}



void Value::setValue(const float & val) {
    type_ = TypeId::FLOAT32;
    value_.unencrypted_val = val;
}

void Value::setValue(const std::string & aString) {
        type_ = TypeId::VARCHAR;
        value_.unencrypted_val = aString;
}


    void Value::setValue(const emp::Bit & val) {
        type_ = TypeId::ENCRYPTED_BOOLEAN;
        value_.emp_bit_ = val.bit;
    }
    void Value::setValue(TypeId type, const emp::Integer & val) {
        type_ = type;
        value_.emp_integer_ = val;
    }

    void Value::setValue(const emp::Float & val) {
        type_ = TypeId::ENCRYPTED_FLOAT32;
        value_.emp_float32_ = val;
    }





    std::ostream &operator<<(std::ostream &os, const Value &aValue) {
        return os << aValue.toString();
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
                string valueStr = std::to_string(value);
                valueStr.erase ( valueStr.find_last_not_of('0') + 1, std::string::npos );
                if(valueStr[valueStr.size()-1] == '.') valueStr.erase(valueStr.find('.'));
                return valueStr;

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
    return string("");
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

    void Value::serialize(int8_t *dst) const {

            size_t valSize = TypeUtilities::getTypeSize(type_)/8;
        if(type_ == TypeId::ENCRYPTED_VARCHAR) {
            valSize *= getEmpInt().bits.size();
        }



        switch (type_) {
                case vaultdb::types::TypeId::BOOLEAN:
                    *dst = getBool();
                    break;
                case vaultdb::types::TypeId::INTEGER32: {
                    int32_t value = getInt32();
                    memcpy(dst, (int8_t *) &value, valSize);
                    break;
                }
                case vaultdb::types::TypeId::NUMERIC:
                case vaultdb::types::TypeId::FLOAT32: {
                    float value = getFloat32();
                    memcpy(dst, (int8_t *) &value, valSize);
                    break;
                }

                case vaultdb::types::TypeId::DATE:
                case vaultdb::types::TypeId::INTEGER64: {
                    int64_t value = getInt64();
                    memcpy(dst, (int8_t *) &value, valSize);
                    break;
                }

                case vaultdb::types::TypeId::VARCHAR: {
                    std::string valueStr = getVarchar();
                    memcpy(dst, (int8_t *) valueStr.c_str(), valueStr.size());
                    break;
                }


                case TypeId::ENCRYPTED_BOOLEAN: {
                    emp::Bit bit = getEmpBit();
                    memcpy(dst, (int8_t *) &(bit.bit), valSize);
                    break;
                }

                case TypeId::ENCRYPTED_INTEGER32:
                case TypeId::ENCRYPTED_INTEGER64:
                case TypeId::ENCRYPTED_VARCHAR:
                    memcpy(dst, (int8_t *) getEmpInt().bits.data(), valSize);
                    break;

                case TypeId::ENCRYPTED_FLOAT32: {
                    memcpy(dst, (int8_t *) getEmpFloat32().value.data(), valSize);
                    break;
                }


                default: // unsupported type
                    throw;
            }


        }


        // best effort on encrypted data
    Value types::Value::deserialize(QueryFieldDesc desc, int8_t *cursor) {

        uint32_t valSize = desc.size() / 8;

        switch (desc.getType()) {
            case vaultdb::types::TypeId::BOOLEAN:
                return Value(*(bool *) cursor);
            case vaultdb::types::TypeId::INTEGER32:
                return Value(*(int32_t *) cursor);
            case vaultdb::types::TypeId::NUMERIC:
            case vaultdb::types::TypeId::FLOAT32:
                return Value(*(float_t *) cursor);
            case vaultdb::types::TypeId::DATE:
            case vaultdb::types::TypeId::INTEGER64:
                return Value(desc.getType(), *(int64_t *) cursor);
            case vaultdb::types::TypeId::VARCHAR: {
                string resultStr((char *) cursor, desc.getStringLength()); // string is null-terminated
                return Value(resultStr);
            }
            // need this for final hand-off
            case TypeId::ENCRYPTED_BOOLEAN: {
              emp::Bit myBit(*(emp::block *) cursor);
              return Value(myBit);
            }
            case TypeId::ENCRYPTED_INTEGER32:
            case TypeId::ENCRYPTED_INTEGER64:
            case TypeId::ENCRYPTED_VARCHAR: {
                emp::Integer myInt(valSize, 0, emp::PUBLIC);
                memcpy(myInt.bits.data(), cursor, valSize*sizeof(Bit));
                return Value(desc.getType(), myInt);
            }
            case TypeId::ENCRYPTED_FLOAT32: {
                emp::Float aFloat(0, emp::PUBLIC);
                memcpy(aFloat.value.data(), (emp::Bit *) cursor, valSize*sizeof(Bit) );
                return Value(aFloat);

            }
            default:
                throw std::invalid_argument("Could not deserialize " + TypeUtilities::getTypeString(desc.getType()));

        }
            return Value(0);

        }


    Value types::Value::deserialize(QueryFieldDesc desc, Bit *cursor) {

        uint32_t valSize = desc.size();


        switch (desc.getType()) {
            case TypeId::ENCRYPTED_BOOLEAN: {
                emp::Bit myBit( *cursor);
                return Value(myBit);
            }
            case TypeId::ENCRYPTED_INTEGER32:
            case TypeId::ENCRYPTED_INTEGER64: {
                Integer myInt(valSize, 0, PUBLIC);
                memcpy(myInt.bits.data(), cursor, valSize*sizeof(Bit));
                return Value(desc.getType(), myInt);
            }
            case TypeId::ENCRYPTED_VARCHAR: {
                Integer myInt(valSize, 0, PUBLIC);
                // need to reverse the character order to make >, <, = work
                //vector<Bit> reversed(valSize);

                // start out with the last 8 bits, work backwards
                // TODO: codify the reversal of these bits
                // this should happen in the generate secret shares step.
                Bit *readCursor = cursor + valSize;
                Bit *writeCursor = myInt.bits.data();
                while(readCursor != cursor) {
                    readCursor -= 8;
                    memcpy(writeCursor, readCursor, 8 * sizeof(Bit));
                    writeCursor += 8;

                }


                return Value(TypeId::ENCRYPTED_VARCHAR, myInt);

            }
            case TypeId::ENCRYPTED_FLOAT32: {
                emp::Float aFloat(0, emp::PUBLIC);
                memcpy(aFloat.value.data(), (emp::Bit *) cursor, valSize*sizeof(Bit));
                return Value(aFloat);

            }
            default:
                throw std::invalid_argument("Could not deserialize " + TypeUtilities::getTypeString(desc.getType()));

        }
        return Value(0);
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
                return types::Value( dst);


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
                // hackity, hack, hack!
                std::string bitString = this->getEmpInt().reveal<std::string>(emp::PUBLIC);
                std::string::iterator strPos = bitString.begin();
                for(int i =  0; i < bitCount; ++i) {
                    bools[i] = (*strPos == '1');
                    ++strPos;
                }

                vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, bitCount);
                decodedBytesVector.resize(byteCount + 1);
                decodedBytesVector[byteCount] = '\0';
                string dst((char * ) decodedBytesVector.data());
                std::reverse(dst.begin(), dst.end());
                Value dstValue(dst);

                delete[] bools;

                return dstValue;
            }

            default: // all other types are unencrypted, so just copy out the value
                if(!TypeUtilities::isEncrypted(getType())) {
                    return types::Value(*this);
                }
                else {
                    throw;
                }
        };
}



    Value Value::obliviousIf(const Value &cmp, Value &lhs, Value &rhs) {
        assert(cmp.getType() == TypeId::ENCRYPTED_BOOLEAN);
        return obliviousIf(cmp.getEmpBit(), lhs, rhs);
    }

    Value Value::toFloat(const Value &src) {
        assert(!TypeUtilities::isEncrypted(src.getType()));
        switch(src.getType()) {
            case TypeId::BOOLEAN:
                return Value((float_t) src.getBool());
            case TypeId::INTEGER32:
                return Value((float_t) src.getInt32());
            case TypeId::INTEGER64:
                return Value((float_t) src.getInt64());
            case TypeId::FLOAT32:
                return Value(src.getFloat32());
            default:
                throw std::invalid_argument("Cannot convert value of type " +
                                                    TypeUtilities::getTypeString(src.getType()) + " to float.");

        }

    }




} // namespace vaultdb::types

