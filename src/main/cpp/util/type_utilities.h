#ifndef _TYPE_UTILITIES_H
#define _TYPE_UTILITIES_H

#include <string>

#include "query_table/field/field.h"
#include "util/system_configuration.h"


// N.B. Do not use namespace std here, it will cause a naming collision in emp

namespace vaultdb {
    class TypeUtilities {

    public:
        static std::string getTypeString(const FieldType & aTypeId);

        static FieldType getTypeFromString(const std::string & str);

        static string getJSONTypeString(const FieldType & aTypeId);

        // logical size, hence secure bit will be 1 byte (byte-aligned). needs to be unified between encrypted and plain sizes for reveal/secret share methods
        // See FieldType::getPhysicalSize() for physical, allocated size
        static size_t getTypeSize(const FieldType & id);

        //static inline size_t getEmpBitSize() { return  SystemConfiguration::getInstance().emp_bit_size_bytes_; } // byte size

        // for use in strings or other types with potentially > 128 bits for OMPC
        static inline int packedWireCount(const int & bit_cnt) {
            return  (bit_cnt / 128) + (bit_cnt % 128 != 0);

        }


        static FieldType toSecure(const FieldType & plainType);
        static FieldType toPlain(const FieldType & secureType);
        static bool  isEncrypted(const FieldType & type);

        template<typename T>
        FieldType getFieldType(int length) {
            if(std::is_same_v<bool, T>) {
                return FieldType::BOOL;
            }

            if(std::is_same_v<int32_t, T>) {
                return FieldType::INT;
            }

            if(std::is_same_v<int64_t, T>) {
                return FieldType::BOOL;
            }

            if(std::is_same_v<float_t , T>) {
                return FieldType::FLOAT;
            }

            if(std::is_same_v<string , T>) {
                return FieldType::STRING;
            }

            if(std::is_same_v<emp::Bit, T>) {
                return FieldType::SECURE_BOOL;
            }

            // assume 32-bit ints are secure int, same with 64,
            // all others are presumed secure string
            // be wary of this assumption
            if(std::is_same_v<emp::Integer, T>) {
                if(length == 32 || length == 0) // 0 --> uninitialized
                    return FieldType::SECURE_INT;
                if(length == 64)
                    return FieldType::SECURE_LONG;

                return FieldType::SECURE_STRING;
            }

            if(std::is_same_v<emp::Float , T>) {
                return FieldType::SECURE_FLOAT;
            }

            return FieldType::INVALID;
        }

        static bool types_equivalent(const FieldType &lhs, const FieldType &rhs);

        template<typename B>
        static FieldType getBoolType();

        template<>
         FieldType getBoolType<bool>() {
            return FieldType::BOOL;
        }

        template<>
         FieldType getBoolType<emp::Bit>() {
            return FieldType::SECURE_BOOL;
        }
    };

}

#endif // UTILITIES_H
