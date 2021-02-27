#include "int_field.h"

using namespace vaultdb;

vaultdb::IntField &vaultdb::IntField::operator=(const vaultdb::IntField &other) {
    this->payload = other.payload;
    return *this;
}

// TODO: push this up to parent class
void vaultdb::IntField::serialize(int8_t *dst) const {
    memcpy(dst, (int8_t *) &payload, size()/8);
}

vaultdb::IntField vaultdb::IntField::deserialize(const int8_t *cursor) {
    IntField ret;
    memcpy((int8_t *) &ret.payload, cursor, ret.size()/8);
    return ret;
}

std::shared_ptr<Field> vaultdb::IntField::reveal() const {
   return  std::shared_ptr<Field>(new IntField(*this));
}
