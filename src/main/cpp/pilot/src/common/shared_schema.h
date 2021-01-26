#ifndef _SHARED_SCHEMA_H
#define _SHARED_SCHEMA_H

#include <query_table/query_schema.h>

using namespace vaultdb;
using namespace vaultdb::types;

class SharedSchema {
public:
    static QuerySchema getInputSchema();
};


#endif //VAULTDB_EMP_SHARED_SCHEMA_H
