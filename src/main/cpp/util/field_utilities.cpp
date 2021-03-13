#include "field_utilities.h"

#include <query_table/field/bool_field.h>
#include <query_table/field/int_field.h>
#include <query_table/field/long_field.h>
#include <query_table/field/float_field.h>
#include <query_table/field/string_field.h>

#include <query_table/field/secure_bool_field.h>
#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_long_field.h>
#include <query_table/field/secure_float_field.h>
#include <query_table/field/secure_string_field.h>

using namespace vaultdb;

size_t FieldUtilities::getPhysicalSize(const FieldType &id, const size_t &strLength) {
    switch (id) {
        case FieldType::SECURE_BOOL:
            return sizeof(emp::Bit);
        case FieldType::BOOL:
            return sizeof(bool); // stored size when we serialize it
        case FieldType::SECURE_FLOAT:
            return sizeof(emp::Float);
        case FieldType::SECURE_INT:
            return sizeof(emp::Integer(32, 0));
        case FieldType::INT:
            return sizeof(int);
        case FieldType::FLOAT:
            return sizeof(float);
        case FieldType::SECURE_LONG:
            return sizeof(emp::Integer(64, 0));

        case FieldType::LONG:
            return sizeof(long long int);

        case FieldType::SECURE_STRING:
            return sizeof(emp::Integer(8*strLength, 0));
        case FieldType::STRING: {
            return sizeof(signed char) * (strLength + 1); // just storing the character array, +1 for '\0'
        }
        case FieldType::INVALID:
        default: // unsupported type
            throw;

    }
}

bool FieldUtilities::equal(const Field *lhs, const Field *rhs) {

    assert(lhs->getType() == rhs->getType());

    switch(lhs->getType()) {
        case FieldType::BOOL: {
            auto lhsField = static_cast<const BoolField *>(lhs);
            auto rhsField = static_cast<const BoolField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::INT: {
            auto lhsField = static_cast<const IntField *>(lhs);
            auto rhsField = static_cast<const IntField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::LONG: {
            auto lhsField = static_cast<const LongField *>(lhs);
            auto rhsField = static_cast<const LongField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::FLOAT: {
            auto lhsField = static_cast<const FloatField *>(lhs);
            auto rhsField = static_cast<const FloatField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::STRING: {
            auto lhsField = static_cast<const StringField *>(lhs);
            auto rhsField = static_cast<const StringField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }


    }
    return true; // encrypted case, can't check!
}

emp::Bit FieldUtilities::secureEqual(const Field *lhs, const Field *rhs) {
    assert(lhs->getType() == rhs->getType());

    switch(lhs->getType()) {
        case FieldType::SECURE_BOOL: {
            auto lhsField = static_cast<const SecureBoolField *>(lhs);
            auto rhsField = static_cast<const SecureBoolField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::SECURE_INT: {
            auto lhsField = static_cast<const SecureIntField *>(lhs);
            auto rhsField = static_cast<const SecureIntField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::SECURE_LONG: {
            auto lhsField = static_cast<const SecureLongField *>(lhs);
            auto rhsField = static_cast<const SecureLongField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::SECURE_FLOAT: {
            auto lhsField = static_cast<const SecureFloatField *>(lhs);
            auto rhsField = static_cast<const SecureFloatField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }
        case FieldType::SECURE_STRING: {
            auto lhsField = static_cast<const SecureStringField *>(lhs);
            auto rhsField = static_cast<const SecureStringField *>(rhs);
            return (*lhsField == *rhsField).getPayload();
        }

        default:
            return emp::Bit(equal(lhs, rhs));
    }
}

bool FieldUtilities::geq(const Field *lhs, const Field *rhs) {
    assert(lhs->getType() == rhs->getType());

    switch(lhs->getType()) {
        case FieldType::BOOL: {
            auto lhsField = static_cast<const BoolField *>(lhs);
            auto rhsField = static_cast<const BoolField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::INT: {
            auto lhsField = static_cast<const IntField *>(lhs);
            auto rhsField = static_cast<const IntField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::LONG: {
            auto lhsField = static_cast<const LongField *>(lhs);
            auto rhsField = static_cast<const LongField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::FLOAT: {
            auto lhsField = static_cast<const FloatField *>(lhs);
            auto rhsField = static_cast<const FloatField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::STRING: {
            auto lhsField = static_cast<const StringField *>(lhs);
            auto rhsField = static_cast<const StringField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }


    }
    return true; // encrypted case, can't check!
}

emp::Bit FieldUtilities::secureGeq(const Field *lhs, const Field *rhs)  {
    assert(lhs->getType() == rhs->getType());

    switch(lhs->getType()) {
        case FieldType::SECURE_BOOL: {
            auto lhsField = static_cast<const SecureBoolField *>(lhs);
            auto rhsField = static_cast<const SecureBoolField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::SECURE_INT: {
            auto lhsField = static_cast<const SecureIntField *>(lhs);
            auto rhsField = static_cast<const SecureIntField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::SECURE_LONG: {
            auto lhsField = static_cast<const SecureLongField *>(lhs);
            auto rhsField = static_cast<const SecureLongField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::SECURE_FLOAT: {
            auto lhsField = static_cast<const SecureFloatField *>(lhs);
            auto rhsField = static_cast<const SecureFloatField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }
        case FieldType::SECURE_STRING: {
            auto lhsField = static_cast<const SecureStringField *>(lhs);
            auto rhsField = static_cast<const SecureStringField *>(rhs);
            return (*lhsField >= *rhsField).getPayload();
        }

        default:
            return emp::Bit(geq(lhs, rhs));
    }

}


