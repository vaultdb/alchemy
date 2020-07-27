#ifndef TYPE_ID_H
#define TYPE_ID_H


namespace vaultdb::types {
enum class TypeId {
    INVALID = 0,
    BOOLEAN,
    INTEGER32,
    INTEGER64,
    FLOAT32,
    FLOAT64,
    VAULT_DOUBLE, // what is this?
    NUMERIC, // becomes ENCRYPTED_FLOAT32 in emp-land
    TIMESTAMP, // alias for INT64
    TIME,  // alias for INT32
    DATE, // alias for INT32
    VARCHAR,
    ENCRYPTED_INTEGER32,
    ENCRYPTED_INTEGER64,
    ENCRYPTED_BOOLEAN,
    ENCRYPTED_FLOAT32,
    ENCRYPTED_FLOAT64,
    ENCRYPTED_VARCHAR, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
    ENCRYPTED_TIMESTAMP, // alias to ENCRYPTED_INTEGER64
    ENCRYPTED_TIME, // alias to ENCRYPTED_INTEGER32
    ENCRYPTED_DATE // alias to ENCRYPTED_INTEGER32
};



} // namespace vaultdb::types

#endif //TYPE_ID_H
