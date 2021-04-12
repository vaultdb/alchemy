#ifndef _FIELD_UTILITIES_H
#define _FIELD_UTILITIES_H


#include<type_traits>
#include <query_table/field/field.h>
#include <query_table/query_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/field/field_type.h"

namespace vaultdb {

    class FieldUtilities {
    public:
        // size is in bytes
        static size_t getPhysicalSize(const FieldType &id, const size_t & strLength = 0);
        static emp::Float toFloat(const emp::Integer &input);
        //static void secret_share(const QueryTuple<bool> *src_tuple,  const std::shared_ptr<QuerySchema> &src_schema, QueryTuple<emp::Bit> & dst_tuple, const int &myParty, const int &dstParty);

        template<typename B>
        static inline bool validateTypeAlignment(const Field<B> &field) {

            switch(field.getType()) {
                case FieldType::BOOL:
                    return field.payload_.which() == 0;
                case FieldType::INT:
                    return field.payload_.which() == 1;
                case FieldType::LONG:
                    return field.payload_.which() == 2;
                case FieldType::FLOAT:
                    return field.payload_.which() == 3;
                case FieldType::STRING:
                    return field.payload_.which() == 4;
                case FieldType::SECURE_BOOL:
                    return field.payload_.which() == 5;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING:
                    return field.payload_.which() == 6;
                case FieldType::SECURE_FLOAT:
                    return field.payload_.which() == 7;
                default:
                    return true; // uninitialized for FieldType::INVALID

            }

        }


        static void secret_share_send(const QueryTuple<bool> &src_tuple, QueryTuple<Bit> &dst_tuple, const int &dst_party);
        static void secret_share_recv(SecureTuple &dst_tuple, const int &dst_party);

        // for template<typename T> debug
        static bool extract_bool(const emp::Bit & b) { return b.reveal();  }
        static bool extract_bool(const bool & b) { return b; }

        static bool select(const bool & choice, const bool & lhs, const bool & rhs);
        static emp::Bit select(const emp::Bit & choice, const emp::Bit & lhs, const emp::Bit & rhs);
    };


}
#endif //_FIELD_UTILITIES_H
