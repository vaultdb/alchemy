#include "field_utilities.h"

#include <query_table/field/bool_field.h>
#include <query_table/field/int_field.h>
#include <query_table/field/long_field.h>
#include <query_table/field/float_field.h>
#include <query_table/field/string_field.h>

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

bool FieldUtilities::equals(const Field *lhs, const Field *rhs) {

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

