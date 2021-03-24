#ifndef _FIELDTYPE_H
#define _FIELDTYPE_H


namespace vaultdb {
    enum class FieldType {
        INVALID = 0,
        BOOL,
        DATE, // DATE is just a stand-in for long
        INT,
        LONG,
        FLOAT,
        STRING,
        SECURE_INT,
        SECURE_LONG,
        SECURE_BOOL,
        SECURE_FLOAT,
        SECURE_STRING, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
    };


}

#endif //_FIELDTYPE_H
