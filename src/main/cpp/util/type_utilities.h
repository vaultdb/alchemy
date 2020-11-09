#ifndef _TYPE_UTILITIES_H
#define _TYPE_UTILITIES_H

#include <string>
#include "query_table/query_table.h"
#include "query_table/types/type_id.h"

// N.B. Do not use namespace std here, it will cause a naming collision in emp

namespace vaultdb {
    class TypeUtilities {

    public:
        static std::string getTypeIdString(vaultdb::types::TypeId aTypeid);

        static size_t getTypeSize(types::TypeId id);

        // are these two types implemented identically?
        // e.g., TypeId::NUMERIC is equal to TypeId::FLOAT32
        static bool typesEqual(const vaultdb::types::TypeId  & lhs, const vaultdb::types::TypeId  & rhs);

        // when reading data from ascii sources like csv
        static types::Value decodeStringValue(const std::string & strValue, const QueryFieldDesc &fieldSpec);

        static types::Value getZero(types::TypeId & aType);

        types::Value getOne(types::TypeId &aType);
    };

}

#endif // UTILITIES_H
