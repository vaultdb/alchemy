#ifndef _FIELD_UTILITIES_H
#define _FIELD_UTILITIES_H


#include<type_traits>
#include <query_table/field/field.h>

#include "query_table/field/field_type.h"

namespace vaultdb {

    class FieldUtilities {
    public:
        // size is in bytes
        static size_t getPhysicalSize(const FieldType &id, const size_t & strLength = 0);
        static bool equal(const Field *lhs, const Field *rhs);
        static bool geq(const Field *lhs, const Field *rhs);
        static bool gt(const Field * lhs, const Field *rhs);

        static emp::Bit secureEqual(const Field *lhs, const Field *rhs);
        static emp::Bit secureGeq(const Field *lhs, const Field *rhs);
        static emp::Bit secureGt(const Field *lhs, const Field *rhs);

    };


}
#endif //_FIELD_UTILITIES_H
