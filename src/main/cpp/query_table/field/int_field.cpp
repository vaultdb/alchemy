#include "int_field.h"

vaultdb::IntField &vaultdb::IntField::operator=(const vaultdb::IntField &other) {
    this->payload = other.payload;
    return *this;
}

void vaultdb::IntField::serialize(int8_t *dst) const {
    memcpy(dst, (int8_t *) &payload, size()/4);
}

vaultdb::IntField vaultdb::IntField::deserialize(const int8_t *cursor) {
    IntField ret;
    std::memcpy((int8_t *) &ret.payload, cursor, ret.size()/8);
    return ret;
}
