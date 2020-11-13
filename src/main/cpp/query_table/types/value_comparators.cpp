//
// Created by Jennie Rogers on 8/14/20.
//
// based on emp's comparable.h

#include "value.h"
#include <emp-tool/circuits/float32.h>

using vaultdb::types::Value;
using namespace vaultdb;

Value Value::operator>=(const Value &rhs) const {
    types::TypeId opType = getType();

    // do not compare if they are not the same type
    if(opType != rhs.getType())
        return false;


    switch (opType) {

        case TypeId::BOOLEAN: {
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal >= rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal >= rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal >= rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR:
         {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(lhsVal >= rhsVal);
        }
        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();
            emp::Bit lt = lhsVal.less_than(rhsVal);
            return Value(!lt); // >=
        }

        case TypeId::ENCRYPTED_BOOLEAN: {
            emp::Bit lhsVal = this->getEmpBit();
            emp::Bit rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, true);
            lhsInt.bits[0] = lhsVal;
            emp::Integer rhsInt(1, true);
            lhsInt.bits[0] = rhsVal;
            return Value(lhsInt >= rhsInt);
        }

        case TypeId::VARCHAR: {
            std::string lhsVal = this->getVarchar();
            std::string rhsVal = rhs.getVarchar();
            return Value(lhsVal >= rhsVal);
        }

        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal >= rhsVal);
        }

        case TypeId::INVALID:
            break;
        default:
            throw;
    }
}

Value Value::operator>(const Value &rhs) const {

    return !(rhs >= *static_cast<const Value*>(this));
}

Value Value::operator<(const Value &rhs) const {
  return !((*static_cast<const Value *>(this)) >= rhs);
}

Value Value::operator<=(const Value &rhs) const {
  return rhs >= *static_cast<const Value *>(this);
}
Value Value::operator==(const Value &rhs) const {
    types::TypeId opType = getType();

    // do not compare if they are not the same type
    if(opType != rhs.getType())
        return false;

    switch (opType) {

        case TypeId::BOOLEAN: {
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal ==  rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal == rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal == rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            emp::Bit eq = lhsVal.equal(rhsVal);
            return Value(eq);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();

            emp::Bit res(lhsVal.equal(rhsVal));
            return Value(res);
        }

        case TypeId::ENCRYPTED_BOOLEAN: {
            emp::Bit lhsVal = this->getEmpBit();
            emp::Bit rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, true);
            lhsInt.bits[0] = lhsVal;
            emp::Integer rhsInt(1, true);
            rhsInt.bits[0] = rhsVal;
            return Value(lhsInt == rhsInt);
        }

        case TypeId::VARCHAR: {
            std::string lhsVal = this->getVarchar();
            std::string rhsVal = rhs.getVarchar();
            return Value(lhsVal == rhsVal);
        }

        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            if(abs(lhsVal - rhsVal) <= 0.1) // wide tolerances because of deltas in how psql will calculate this and what we get from extracting fp #s
                return Value(true);

            return Value(false);
        }


        case TypeId::INVALID:
            break;
        default:
            throw;
    }
}

Value Value::operator!=(const Value &rhs) const {
  Value isEqual(*this == rhs);
  if (isEqual.getType() == TypeId::ENCRYPTED_BOOLEAN) {
    emp::Bit payload = isEqual.getEmpBit();
    return Value(!payload);
  }

  bool payload = isEqual.getBool();
  return Value(!payload);
}

// for use only with bool and emp::Bit
Value vaultdb::types::Value::operator!() const {
  TypeId valType = getType();

  assert(valType == TypeId::ENCRYPTED_BOOLEAN || valType == TypeId::BOOLEAN);

  if (valType == TypeId::ENCRYPTED_BOOLEAN) {
    emp::Bit payload = getEmpBit();
    payload = !payload;
    return Value(payload); // setting up a new shared_ptr
  }

  bool payload = getBool();
  return Value(!payload);
}

// lhs = (cmp) ? rhs  : lhs
void vaultdb::types::Value::compareAndSwap(Value &lhs, Value &rhs,
                                           const emp::Bit &cmp) {

  types::TypeId opType = lhs.getType();

  assert(lhs.getType() ==
         rhs.getType()); // do not compare if they are not the same type
  assert(lhs.is_encrypted_);
  assert(rhs.is_encrypted_); // don't need this for the plaintext setting

  switch (opType) {

  case TypeId::ENCRYPTED_INTEGER32:
  case TypeId::ENCRYPTED_INTEGER64:
  case TypeId::ENCRYPTED_VARCHAR: {
    emp::Integer lhsVal = lhs.getEmpInt();
    emp::Integer rhsVal = rhs.getEmpInt();
    emp::swap(cmp, lhsVal, rhsVal);
    lhs.setValue(opType, lhsVal);
    rhs.setValue(opType, rhsVal);
    break;
  }

  case TypeId::ENCRYPTED_FLOAT32: {
    emp::Float lhsVal = lhs.getEmpFloat32();
    emp::Float rhsVal = rhs.getEmpFloat32();
    emp::swap(cmp, lhsVal, rhsVal);
    lhs.setValue(lhsVal);
    rhs.setValue(rhsVal);
    break;
  }

  case TypeId::ENCRYPTED_BOOLEAN: {
    emp::Bit lhsVal = lhs.getEmpBit();
    emp::Bit rhsVal = rhs.getEmpBit();
    emp::swap(cmp, lhsVal, rhsVal);
    lhs.setValue(lhsVal);
    rhs.setValue(rhsVal);
    break;
  }
  default:
    throw;
  }
}
Value types::Value::obliviousIf(const emp::Bit &cmp, Value &lhs, Value &rhs) {

  types::TypeId opType = lhs.getType();

  assert(lhs.getType() ==
         rhs.getType()); // do not compare if they are not the same type
  assert(lhs.is_encrypted_);
  assert(rhs.is_encrypted_); // don't need this for the plaintext setting

  switch (opType) {

  case TypeId::ENCRYPTED_INTEGER32:
  case TypeId::ENCRYPTED_INTEGER64:
  case TypeId::ENCRYPTED_VARCHAR: {
    emp::Integer lhsVal = lhs.getEmpInt();
    emp::Integer rhsVal = rhs.getEmpInt();
    emp::Integer result = emp::If(cmp, lhsVal, rhsVal);
    return Value(opType, result);
  }

  case TypeId::ENCRYPTED_FLOAT32: {
    emp::Float lhsVal = lhs.getEmpFloat32();
    emp::Float rhsVal = rhs.getEmpFloat32();
    emp::Float result = emp::If(cmp, lhsVal, rhsVal);
    return Value(result);
  }

  case TypeId::ENCRYPTED_BOOLEAN: {
    emp::Bit lhsVal = lhs.getEmpBit();
    emp::Bit rhsVal = rhs.getEmpBit();
    emp::Bit result = emp::If(cmp, lhsVal, rhsVal);
    return Value(result);
  }
  default:
    throw;
  }


}

types::Value::Value(const types::TypeId &type, const int64_t &val) {

  type_ = type;
  is_encrypted_ = false;
  setValue(val);
}

Value types::Value::operator|(const Value &rhs) const {
  assert(rhs.getType() == this->getType()); // must be of same type
  assert(getType() == TypeId::BOOLEAN ||
         getType() == TypeId::ENCRYPTED_BOOLEAN);

  if (getType() == TypeId::ENCRYPTED_BOOLEAN) {
    return Value(this->getEmpBit() | rhs.getEmpBit());
  }
  bool result = this->getBool() || rhs.getBool();
  return Value(result);
}

Value types::Value::operator^(const Value &rhs) const {
  assert(rhs.getType() == this->getType()); // must be of same type
  assert(getType() == TypeId::BOOLEAN ||
         getType() == TypeId::ENCRYPTED_BOOLEAN);

  if (getType() == TypeId::ENCRYPTED_BOOLEAN) {
    return Value(this->getEmpBit() ^ rhs.getEmpBit());
  }

  bool result = this->getBool() ^ rhs.getBool();
  return Value(result);
}

Value types::Value::operator&(const Value &rhs) const {

  assert(rhs.getType() == this->getType()); // must be of same type
  assert(getType() == TypeId::BOOLEAN ||
         getType() == TypeId::ENCRYPTED_BOOLEAN);

  if (getType() == TypeId::ENCRYPTED_BOOLEAN) {
    return Value(this->getEmpBit() & rhs.getEmpBit());
  }
  bool result = this->getBool() && rhs.getBool();
  return Value(result);
}


