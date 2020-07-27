#ifndef TYPE_ID_H
#define TYPE_ID_H


namespace vaultdb::types {
    // timestamps are aliased to INT64, TIME and DATE are aliased to INT32s
enum class TypeId {
    INVALID = 0,
    BOOLEAN,
    INTEGER32,
    INTEGER64,
    FLOAT32,
    FLOAT64,
    NUMERIC, // alias for FLOAT32, becomes ENCRYPTED_FLOAT32 in emp-land
    VARCHAR,
    ENCRYPTED_INTEGER32,
    ENCRYPTED_INTEGER64,
    ENCRYPTED_BOOLEAN,
    ENCRYPTED_FLOAT32,
    ENCRYPTED_FLOAT64,
    ENCRYPTED_VARCHAR, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
};



} // namespace vaultdb::types

#endif //TYPE_ID_H
