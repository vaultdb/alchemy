//
// Created by Jennie Rogers on 8/14/20.
//
// based on emp's comparable.h

#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "value.h"

using vaultdb::types::Value;

Value Value::operator>=(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

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
      //  case TypeId::ENCRYPTED_VARCHAR:
         {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(lhsVal >= rhsVal);
        }
        case TypeId::ENCRYPTED_VARCHAR:
            {
                emp::Integer lhsVal = this->getEmpInt();
                emp::Integer rhsVal = rhs.getEmpInt();
                size_t charCount = lhsVal.size() / 8;
                assert(lhsVal.size() % 8 == 0); // no modulus

                // test it one character at a time
                // split it into an array of chars

                emp::Integer lhsChars[charCount];
                emp::Integer rhsChars[charCount];

                for(int i = 0; i < charCount; ++i) {
                    lhsChars[i] = emp::Integer(8, lhsVal.bits + i * 8);
                    rhsChars[i] = emp::Integer(8, rhsVal.bits + i * 8);
                }

                emp::Bit geq = lhsChars[0].geq(rhsChars[0]); // bootstrap
                emp::Bit eq = lhsChars[0].equal(rhsChars[0]); // has it been equal so far?

                // while it remains equal
                for(int i = 1; i < charCount; ++i) {
	                // if it's been matched all along, save lhsChar[i] >= rhsChar[i]
                    geq = emp::If(eq, lhsChars[i].geq(rhsChars[i]), geq);
                    eq = eq & lhsChars[i].equal(rhsChars[i]);
	            }
            	return geq;

            }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = this->getEmpFloat32();
            emp::Float32 rhsVal = rhs.getEmpFloat32();
            emp::Bit lt = lhsVal.less_than(rhsVal);
            return Value(!lt); // >=
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = this->getEmpFloat64();
            emp::Float rhsVal = rhs.getEmpFloat64();
            return Value( lhsVal.greater(rhsVal) | (lhsVal.equal(rhsVal)));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            emp::Bit lhsVal = this->getEmpBit();
            emp::Bit rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, &lhsVal);
            emp::Integer rhsInt(1, &rhsVal);
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

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
            return Value(lhsVal >= rhsVal);
        }
        case TypeId::INVALID:
            break;
        default:
            throw;
    }


}

Value Value::operator>(const Value &rhs) const {
    return !(rhs >= *static_cast<const Value *>(this));

}

Value Value::operator<(const Value &rhs) const {
    return !( (*static_cast<const Value*>(this))>= rhs );

}



Value Value::operator<=(const Value &rhs) const {
    return rhs >= *static_cast<const Value*>(this);
}
Value Value::operator==(const Value &rhs) const {
    types::TypeId opType = getType();


    assert(opType == rhs.getType()); // do not compare if they are not the same type


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
            return Value(lhsVal == rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = this->getEmpFloat32();
            emp::Float32 rhsVal = rhs.getEmpFloat32();
            return Value(lhsVal.equal(rhsVal));
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = this->getEmpFloat64();
            emp::Float rhsVal = rhs.getEmpFloat64();
            return Value(lhsVal.equal(rhsVal));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            emp::Bit lhsVal = this->getEmpBit();
            emp::Bit rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, &lhsVal);
            emp::Integer rhsInt(1, &rhsVal);
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

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
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
    if(isEqual.getType() == TypeId::ENCRYPTED_BOOLEAN) {
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

    if(valType == TypeId::ENCRYPTED_BOOLEAN) {
        emp::Bit payload = getEmpBit();
        payload = !payload;
        return Value(payload); // setting up a new shared_ptr
    }

    bool payload = getBool();
    return Value(!payload);

}

void vaultdb::types::Value::compareAndSwap(Value &lhs, Value &rhs, const emp::Bit &cmp) {


    types::TypeId opType = lhs.getType();

    assert(lhs.getType() == rhs.getType()); // do not compare if they are not the same type
    assert(lhs.is_encrypted_);
    assert(rhs.is_encrypted_);  // don't need this for the plaintext setting

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
            emp::Float32 lhsVal = lhs.getEmpFloat32();
            emp::Float32 rhsVal = rhs.getEmpFloat32();
            emp::swap(cmp, lhsVal, rhsVal);
            lhs.setValue(lhsVal);
            rhs.setValue(rhsVal);
            break;

        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = lhs.getEmpFloat64();
            emp::Float rhsVal = rhs.getEmpFloat64();
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

  assert(lhs.getType() == rhs.getType()); // do not compare if they are not the same type
  assert(lhs.is_encrypted_);
  assert(rhs.is_encrypted_);  // don't need this for the plaintext setting

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
    emp::Float32 lhsVal = lhs.getEmpFloat32();
    emp::Float32 rhsVal = rhs.getEmpFloat32();
    emp::Float32 result = emp::If(cmp, lhsVal, rhsVal);
    return Value(result);
  }
  case TypeId::ENCRYPTED_FLOAT64: {
    emp::Float lhsVal = lhs.getEmpFloat64();
    emp::Float rhsVal = rhs.getEmpFloat64();
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

  return Value();
}

types::Value::Value(const types::TypeId &type, const int64_t &val) {

    type_ = type;
    is_encrypted_ = false;
    value_.unencrypted_val = val;

}

Value types::Value::operator|(const Value &rhs) const {
    assert(rhs.getType() == this->getType());  // must be of same type
    assert(getType() == TypeId::BOOLEAN || getType() == TypeId::ENCRYPTED_BOOLEAN);

    if(getType() == TypeId::ENCRYPTED_BOOLEAN) {
        return Value(this->getEmpBit() | rhs.getEmpBit());
    }
    return Value(this->getBool() | rhs.getBool());
}

Value types::Value::operator^(const Value &rhs) const {
    assert(rhs.getType() == this->getType());  // must be of same type
    assert(getType() == TypeId::BOOLEAN || getType() == TypeId::ENCRYPTED_BOOLEAN);

    if(getType() == TypeId::ENCRYPTED_BOOLEAN) {
        return Value(this->getEmpBit() ^ rhs.getEmpBit());
    }
    return Value(this->getBool() ^ rhs.getBool());
}

Value types::Value::operator&(const Value &rhs) const {

    assert(rhs.getType() == this->getType());  // must be of same type
    assert(getType() == TypeId::BOOLEAN || getType() == TypeId::ENCRYPTED_BOOLEAN);

    if(getType() == TypeId::ENCRYPTED_BOOLEAN) {
        return Value(this->getEmpBit() & rhs.getEmpBit());
    }
    return Value(this->getBool() & rhs.getBool());
}


/*bool vaultdb::types::Value::operator==(const Value &rhs) {
    assert(!this->is_encrypted_ && !rhs.is_encrypted_); // only reveal this for encrypted vals

    Value equality = (*this == rhs);
    return equality.getBool();
}

bool vaultdb::types::Value::operator!=(const Value &rhs) {

    return !(*this == rhs);
}
*/
