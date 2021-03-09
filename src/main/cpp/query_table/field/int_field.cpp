#include "int_field.h"
#include "bool_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const IntField &aValue) {
    return os << aValue.getBaseField();
}


IntField::IntField(const Field &srcField) : field_(srcField) { }

IntField::IntField(const IntField &src) {
    field_ =  Field(src.getBaseField());
}

IntField::IntField(const int32_t &src) {
    field_ = Field::createInt32(src);
}


IntField::IntField(const int8_t *src) {
    field_ = Field::deserialize(FieldType::INT32, 0, src);
}


IntField::IntField(const emp::Integer &src, const int &party) {
    int32_t revealed = src.reveal<int32_t>(party);
    field_ = Field::createInt32(revealed);
}

IntField &IntField::operator=(const IntField &other) {
    if(this == &other) return *this;
    this->field_ = Field(other.getBaseField());
    return *this;
}

BoolField IntField::operator>=(const IntField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField IntField::operator==(const IntField &cmp) const {
    bool res = (field_.getValue<int32_t>()) == (cmp.getBaseField().getValue<int32_t>());
    return  BoolField(res);
}

IntField IntField::select(const BoolField &choice, const IntField &other) const {
    bool selection =  choice.getPayload();
    return selection ? IntField(*this) :  IntField(other);
}

