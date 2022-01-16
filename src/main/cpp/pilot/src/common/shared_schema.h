#ifndef _SHARED_SCHEMA_H
#define _SHARED_SCHEMA_H

#include <query_table/query_schema.h>

using namespace vaultdb;

class SharedSchema {
public:
    static QuerySchema getInputSchema();
    static QuerySchema getPatientSchema();
};


#endif //VAULTDB_EMP_SHARED_SCHEMA_H
