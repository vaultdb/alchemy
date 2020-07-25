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
    };

}

#endif // UTILITIES_H
