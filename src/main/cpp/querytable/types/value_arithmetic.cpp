//
// Created by Jennie Rogers on 8/14/20.
//
#include <util/type_utilities.h>
#include "value.h"

using vaultdb::types::Value;


Value Value::operator+(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: { // treated as an int under the hood
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal +  rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal + rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal + rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(opType, lhsVal + rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();
            return Value(lhsVal + rhsVal);
        }


        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal + rhsVal);
        }

        case TypeId::INVALID:
            break;
        case TypeId::ENCRYPTED_BOOLEAN:  // not supported - semantics not clear yet
        case TypeId::VARCHAR:
        default:
            throw;
    }


}


Value Value::operator-(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: { // treated as an int under the hood
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal -  rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal - rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal - rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(opType, lhsVal - rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();
            return Value(lhsVal - rhsVal);
        }



        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal - rhsVal);
        }


        case TypeId::INVALID:
            break;
        case TypeId::VARCHAR:  // not supported - semantics not clear
        case TypeId::ENCRYPTED_BOOLEAN:
        default:
            throw;
    }

}

Value Value::operator*(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: { // treated as an int under the hood
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal *  rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal + rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal * rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(opType, lhsVal * rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();
            return Value(lhsVal * rhsVal);
        }



        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal * rhsVal);
        }


        case TypeId::INVALID:
            break;
        case TypeId::VARCHAR:  // not supported - semantics not clear
        case TypeId::ENCRYPTED_BOOLEAN:
        default:
            throw;
    }

}

Value Value::operator/(const Value &rhs) const {
    types::TypeId opType = getType();

    assert(opType == rhs.getType()); // do not compare if they are not the same type

    switch (opType) {

        case TypeId::BOOLEAN: { // treated as an int under the hood
            bool lhsVal = getBool();
            bool rhsVal = rhs.getBool();
            return Value(lhsVal /  rhsVal);
        }
        case TypeId::INTEGER32: {
            int32_t lhsVal = this->getInt32();
            int32_t rhsVal = rhs.getInt32();
            return Value(lhsVal / rhsVal);
        }
        case TypeId::INTEGER64: {
            int64_t lhsVal = this->getInt64();
            int64_t rhsVal = rhs.getInt64();
            return Value(lhsVal / rhsVal);
        }

        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_INTEGER64:
        case TypeId::ENCRYPTED_VARCHAR: {
            emp::Integer lhsVal = this->getEmpInt();
            emp::Integer rhsVal = rhs.getEmpInt();
            return Value(opType, lhsVal / rhsVal);
        }

        case TypeId::ENCRYPTED_FLOAT32: {
            emp::Float lhsVal = this->getEmpFloat32();
            emp::Float rhsVal = rhs.getEmpFloat32();
            return Value(lhsVal / rhsVal);
        }



        case TypeId::NUMERIC:
        case TypeId::FLOAT32: {
            float lhsVal = this->getFloat32();
            float rhsVal = rhs.getFloat32();
            return Value(lhsVal / rhsVal);
        }

        
        case TypeId::INVALID:
            break;
        case TypeId::VARCHAR:  // not supported - semantics not clear
        case TypeId::ENCRYPTED_BOOLEAN:
        default:
            throw;
    }

}

