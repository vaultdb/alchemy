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
/*        static Field * equal(const Field *lhs, const Field *rhs);
        static Field * geq(const Field *lhs, const Field *rhs);
        static Field * gt(const Field * lhs, const Field *rhs);*/


    };


}
#endif //_FIELD_UTILITIES_H
