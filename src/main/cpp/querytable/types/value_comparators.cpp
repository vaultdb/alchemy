//
// Created by Jennie Rogers on 8/14/20.
//
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
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = *(this->getEmpInt());
            emp::Integer rhsVal = *(rhs.getEmpInt());
            return Value(lhsVal >= rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = *(this->getEmpFloat32());
            emp::Float32 rhsVal = *(rhs.getEmpFloat32());
            emp::Bit lt = lhsVal.less_than(rhsVal);
            return Value(!lt); // >=
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = *(this->getEmpFloat64());
            emp::Float rhsVal = *(rhs.getEmpFloat64());
            return Value( lhsVal.greater(rhsVal) | (lhsVal.equal(rhsVal)));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            std::shared_ptr<emp::Bit> lhsVal = this->getEmpBit();
            std::shared_ptr<emp::Bit> rhsVal = rhs.getEmpBit();

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
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: {
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal > rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal > rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal > rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = *(this->getEmpInt());
            emp::Integer rhsVal = *(rhs.getEmpInt());
            return Value(lhsVal > rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = *(this->getEmpFloat32());
            emp::Float32 rhsVal = *(rhs.getEmpFloat32());
            emp::Bit leq = lhsVal.less_equal(rhsVal);
            return Value(!leq); // >
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = *(this->getEmpFloat64());
            emp::Float rhsVal = *(rhs.getEmpFloat64());
            return Value( lhsVal.greater(rhsVal));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            std::shared_ptr<emp::Bit> lhsVal = this->getEmpBit();
            std::shared_ptr<emp::Bit> rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, &lhsVal);
            emp::Integer rhsInt(1, &rhsVal);
            return Value(lhsInt > rhsInt);
        }

        case TypeId::VARCHAR: {
            std::string lhsVal = this->getVarchar();
            std::string rhsVal = rhs.getVarchar();
            return Value(lhsVal > rhsVal);
        }

        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal > rhsVal);
        }

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
            return Value(lhsVal > rhsVal);
        }
        case TypeId::INVALID:
            break;
        default:
            throw;
    }

}

Value Value::operator<(const Value &rhs) const {

    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: {
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal < rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal < rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal < rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = *(this->getEmpInt());
            emp::Integer rhsVal = *(rhs.getEmpInt());
            return Value(lhsVal < rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = *(this->getEmpFloat32());
            emp::Float32 rhsVal = *(rhs.getEmpFloat32());
            return Value(lhsVal.less_than(rhsVal)); // <
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = *(this->getEmpFloat64());
            emp::Float rhsVal = *(rhs.getEmpFloat64());
            return Value( (!lhsVal.greater(rhsVal)) ^ (!lhsVal.equal(rhsVal)));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            std::shared_ptr<emp::Bit> lhsVal = this->getEmpBit();
            std::shared_ptr<emp::Bit> rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, &lhsVal);
            emp::Integer rhsInt(1, &rhsVal);
            return Value(lhsInt < rhsInt);
        }

        case TypeId::VARCHAR: {
            std::string lhsVal = this->getVarchar();
            std::string rhsVal = rhs.getVarchar();
            return Value(lhsVal < rhsVal);
        }

        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal < rhsVal);
        }

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
            return Value(lhsVal < rhsVal);
        }
        case TypeId::INVALID:
            break;
        default:
            throw;
    }


}



Value Value::operator<=(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: {
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal <= rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal <= rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal <= rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = *(this->getEmpInt());
            emp::Integer rhsVal = *(rhs.getEmpInt());
            return Value(lhsVal <= rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = *(this->getEmpFloat32());
            emp::Float32 rhsVal = *(rhs.getEmpFloat32());
            return Value(lhsVal.less_equal(rhsVal)); // <=
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = *(this->getEmpFloat64());
            emp::Float rhsVal = *(rhs.getEmpFloat64());
            return Value(!lhsVal.greater(rhsVal));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            std::shared_ptr<emp::Bit> lhsVal = this->getEmpBit();
            std::shared_ptr<emp::Bit> rhsVal = rhs.getEmpBit();

            emp::Integer lhsInt(1, &lhsVal);
            emp::Integer rhsInt(1, &rhsVal);
            return Value(lhsInt <= rhsInt);
        }

        case TypeId::VARCHAR: {
            std::string lhsVal = this->getVarchar();
            std::string rhsVal = rhs.getVarchar();
            return Value(lhsVal <= rhsVal);
        }

        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal <= rhsVal);
        }

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
            return Value(lhsVal <= rhsVal);
        }
        case TypeId::INVALID:
            break;
        default:
            throw;
    }



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
            emp::Integer lhsVal = *(this->getEmpInt());
            emp::Integer rhsVal = *(rhs.getEmpInt());
            return Value(lhsVal == rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float32 lhsVal = *(this->getEmpFloat32());
            emp::Float32 rhsVal = *(rhs.getEmpFloat32());
            return Value(lhsVal.equal(rhsVal)); // <=
        }
        case TypeId::ENCRYPTED_FLOAT64: {
            emp::Float lhsVal = *(this->getEmpFloat64());
            emp::Float rhsVal = *(rhs.getEmpFloat64());
            return Value(lhsVal.equal(rhsVal));
        }
        case TypeId::ENCRYPTED_BOOLEAN: {
            std::shared_ptr<emp::Bit> lhsVal = this->getEmpBit();
            std::shared_ptr<emp::Bit> rhsVal = rhs.getEmpBit();

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
            return Value(lhsVal == rhsVal);
        }

        case TypeId::FLOAT64: {
            double_t lhsVal = this->getFloat64();
            double_t rhsVal = rhs.getFloat64();
            return Value(lhsVal == rhsVal);
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
        std::shared_ptr<emp::Bit> payload = isEqual.getEmpBit();
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
        std::shared_ptr<emp::Bit> payload = getEmpBit();
        *payload = !(*payload);
        return Value(payload.get()); // setting up a new shared_ptr
    }

    bool payload = getBool();
    return Value(!payload);

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
