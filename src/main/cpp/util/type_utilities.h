//
// Created by Jennie Rogers on 7/25/20.
//

#ifndef _TYPE_UTILITIES_H
#define _TYPE_UTILITIES_H

#include <string>
#include <querytable/query_table.h>
#include "querytable/types/type_id.h"

// N.B. Do not use namespace std here, it will cause a naming collision in emp

namespace vaultdb {
    class TypeUtilities {

    public:
        static std::string getTypeIdString(vaultdb::types::TypeId aTypeid);

        static size_t getTypeSize(types::TypeId id);

        // are these two types implemented identically?
        // e.g., TypeId::NUMERIC is equal to TypeId::FLOAT32
        static bool typesEqual(const vaultdb::types::TypeId  & lhs, const vaultdb::types::TypeId  & rhs);
    };

}

#endif // UTILITIES_H
