#include "field_utilities.h"

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

