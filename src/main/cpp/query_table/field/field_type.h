#ifndef _FIELDTYPE_H
#define _FIELDTYPE_H

#include <emp-tool/circuits/Bit.h>
#include <emp-tool/circuits/Float32.h>
#include <emp-tool/circuits/Integer.h>


namespace vaultdb {
    enum class FieldType {
        INVALID = 0,
        BOOL,
        DATE, // DATE is just a stand-in for long
        INT,
        LONG,
        FLOAT,
        STRING,
        SECURE_INT,
        SECURE_LONG,
        SECURE_BOOL,
        SECURE_FLOAT,
        SECURE_STRING, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
    };


    // size is in bytes
    // TODO: give this its own class
    class FieldUtils {
    public:
        static size_t getPhysicalSize(const FieldType &id, const size_t & strLength = 0) {
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
                    return sizeof(int32_t);
                case FieldType::FLOAT:
                    return sizeof(float_t);
                case FieldType::SECURE_LONG:
                    return sizeof(emp::Integer(64, 0));

                case FieldType::LONG:
                    return sizeof(int64_t);

                case FieldType::SECURE_STRING:
                    return sizeof(emp::Integer(8*strLength, 0));
                case FieldType::STRING: {
                    return sizeof(int8_t) * (strLength + 1); // just storing the character array, +1 for '\0'
                }
                case FieldType::INVALID:
                default: // unsupported type
                    throw;

            }
        }

    };


}

#endif //_FIELDTYPE_H
