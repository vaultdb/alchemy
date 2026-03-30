#ifndef _TYPE_UTILITIES_H
#define _TYPE_UTILITIES_H

#include <string>

#include "query_table/field/field.h"


// N.B. Do not use namespace std here, it will cause a naming collision in emp

namespace vaultdb {
    class TypeUtilities {

    public:
        static inline std::string getTypeName(const FieldType & type) {
            switch(type) {
                case FieldType::BOOL:
                    return std::string("bool");
                case FieldType::DATE:
                    return std::string("date");
                case FieldType::INT:
                    return std::string("int32");
                case FieldType::LONG:
                    return std::string("int64");
                case FieldType::FLOAT:
                    return std::string("float");
                case FieldType::STRING:
                    return std::string("varchar");
                case FieldType::SECURE_INT:
                    return std::string("shared-int32");
                case FieldType::SECURE_LONG:
                    return std::string("shared-int64") ;
                case FieldType::SECURE_BOOL:
                    return std::string("shared-bool" );
                case FieldType::SECURE_FLOAT:
                    return std::string("shared-float");

                case FieldType::SECURE_STRING:
                    return std::string("shared-varchar");
                default:
                    std::string FieldTypeStr = std::to_string((int) type);
                    return std::string("unsupported type! " + FieldTypeStr);
            }
        }

        static FieldType getTypeFromString(const std::string & str) {
            string field_str_lower = str;
            transform(field_str_lower.begin(), field_str_lower.end(), field_str_lower.begin(), ::tolower);

            if(field_str_lower == "bool")
                return FieldType::BOOL;

            if(field_str_lower == "date")
                return FieldType::DATE;

            if(field_str_lower == "int32" || field_str_lower == "int")
                return FieldType::INT;

            if(field_str_lower == "int64" || field_str_lower == "long" || field_str_lower == "bigint")
                return FieldType::LONG;

            if(field_str_lower == "float")
                return FieldType::FLOAT;
            if(field_str_lower == "varchar" || field_str_lower == "char")
                return FieldType::STRING;

            if(field_str_lower == "shared-bool")
                return FieldType::SECURE_BOOL;


            if(field_str_lower == "shared-int32" || field_str_lower == "shared-int")
                return FieldType::SECURE_INT;

            if(field_str_lower == "shared-int64"  || field_str_lower == "shared-long")
                return FieldType::SECURE_LONG;

            if(field_str_lower == "shared-float")
                return FieldType::SECURE_FLOAT;
            if(field_str_lower == "shared-varchar" || field_str_lower == "shared-char")
                return FieldType::SECURE_STRING;

            throw std::invalid_argument("Unsupported type " + str);

        }

        static string getJSONTypeString(const FieldType & type_id) {
            switch(type_id) {
                case FieldType::BOOL:
                case FieldType::SECURE_BOOL:
                    return "BOOLEAN";
                case FieldType::INT:
                case FieldType::SECURE_INT:
                    return "INTEGER";
                case FieldType::LONG:
                case FieldType::SECURE_LONG:
                    return "LONG";
                case FieldType::FLOAT:
                case FieldType::SECURE_FLOAT:
                    return "FLOAT";
                case FieldType::STRING:
                case FieldType::SECURE_STRING:
                    return "CHAR";
                case FieldType::DATE:
                    return "DATE";
                default:
                    return "INVALID";

            }
        }

        // size is in bits
        static size_t getTypeSize(const FieldType & id) {
            switch (id) {
                case FieldType::SECURE_BOOL:
                    return 1;
                case FieldType::BOOL:
                    return 8; // stored size when we serialize it

                case FieldType::SECURE_FLOAT:
                case FieldType::SECURE_INT:
                case FieldType::INT:
                case FieldType::FLOAT:
                case FieldType::DATE:
                    return 32;

                case FieldType::SECURE_LONG:
                case FieldType::LONG:
                    return 64;

                case FieldType::SECURE_STRING:
                case FieldType::STRING: // to be multiplied by length in schema for true field size
                    return 8;

                default: // unsupported type
                    throw;

            }
        }


        // for use in strings or other types with potentially > 128 bits for OMPC
        static inline int packedWireCount(const int & bit_cnt) {
            return  (bit_cnt / 128) + (bit_cnt % 128 != 0);

        }


        static FieldType toSecure(const FieldType & plainType) {
            switch(plainType) {
                case FieldType::BOOL:
                    return FieldType::SECURE_BOOL;
                case FieldType::INT:
                    return FieldType::SECURE_INT;
                case FieldType::DATE:
                case FieldType::LONG:
                    return FieldType::SECURE_LONG;
                case FieldType::STRING:
                    return FieldType::SECURE_STRING;
                case FieldType::FLOAT:
                    return FieldType::SECURE_FLOAT;
                default: // already secure
                    return plainType;

            }

        }
        static FieldType toPlain(const FieldType & secureType) {
            switch(secureType) {
                case FieldType::SECURE_BOOL:
                    return FieldType::BOOL;
                case FieldType::SECURE_INT:
                    return FieldType::INT;
                case FieldType::SECURE_LONG:
                    return FieldType::LONG;
                case FieldType::SECURE_FLOAT:
                    return FieldType::FLOAT;
                case FieldType::SECURE_STRING:
                    return FieldType::STRING;
                default: // already plain
                    return secureType;
            }

        }

        static bool  isSecretShared(const FieldType & type) {
            switch(type) {
                case FieldType::SECURE_BOOL:
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_FLOAT:
                case FieldType::SECURE_STRING:
                    return true;
                default:
                    return false;
            };
        }

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

        static bool typesEquivalent(const FieldType &lhs, const FieldType &rhs) {
            if(lhs == rhs)
                return true;

            if(lhs == FieldType::DATE && rhs == FieldType::LONG) return true;
            if(rhs == FieldType::DATE && lhs == FieldType::LONG) return true;

            return false;

        }

        template<typename B>
        static FieldType getBoolType();


    };



}

#endif // UTILITIES_H
