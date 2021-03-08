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
        INT32,
        INT64,
        FLOAT32,
        STRING,
        SECURE_INT32,
        SECURE_INT64,
        SECURE_BOOL,
        SECURE_FLOAT32,
        SECURE_STRING, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
    };


    // size is in bytes
    // TODO: give this its own class
    class FieldUtils {
    public:
        static size_t getPhysicalSize(const FieldType &id) {
            switch (id) {
                case FieldType::SECURE_BOOL:
                    return sizeof(emp::Bit);
                case FieldType::BOOL:
                    return sizeof(bool); // stored size when we serialize it
                case FieldType::SECURE_FLOAT32:
                    return sizeof(emp::Float);
                case FieldType::SECURE_INT32:
                    return sizeof(emp::Integer(32, 0));
                case FieldType::INT32:
                    return sizeof(int32_t);
                case FieldType::FLOAT32:
                    return sizeof(float_t);
                case FieldType::SECURE_INT64:
                    return sizeof(emp::Integer(64, 0));

                case FieldType::INT64:
                    return sizeof(int64_t);

                case FieldType::SECURE_STRING:
                    return sizeof(emp::Integer(8, 0));
                case FieldType::STRING: // to be multiplied by length in schema for true field size
                    return sizeof(std::string("0"));

                case FieldType::INVALID:
                default: // unsupported type
                    throw;

            }
        }

    };


}

#endif //_FIELDTYPE_H
