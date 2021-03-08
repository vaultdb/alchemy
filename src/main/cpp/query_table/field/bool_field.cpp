#include "bool_field.h"

std::ostream &vaultdb::operator<<(std::ostream &os, const vaultdb::BoolField &aValue) {
    return os << aValue.toString();
}

vaultdb::BoolField::BoolField(const vaultdb::Field &srcField) : field_(srcField) {
    payload_ = reinterpret_cast<bool *>(field_.getData());
}

vaultdb::BoolField::BoolField(const vaultdb::BoolField &src) {
    field_ = Field::createBool(src.primitive());
    payload_ = reinterpret_cast<bool *>(field_.getData());
}

vaultdb::BoolField::BoolField(const bool &src) {
    field_ = Field::createBool(src);
    payload_ = reinterpret_cast<bool *>(field_.getData());
}

vaultdb::BoolField::BoolField(const emp::Bit &src, const int &party) {
    *payload_ = src.reveal(party);
}

vaultdb::BoolField &vaultdb::BoolField::operator=(const vaultdb::BoolField &other) {
    if(this == &other) return *this;

    this->field_ = Field(other.getBaseField());
    this->payload_ = reinterpret_cast<bool *>(field_.getData());
    return *this;
}

vaultdb::BoolField vaultdb::BoolField::operator>=(const vaultdb::BoolField &cmp) const {
    bool res = *payload_ >= *cmp.payload_;
    return BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::operator==(const vaultdb::BoolField &cmp) const {
    bool res = *payload_ == *cmp.payload_;
    return  BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::select(const vaultdb::BoolField &choice, const vaultdb::BoolField &other) const {
    bool selection = *(choice.payload_);
    return selection ? BoolField(*this) :  BoolField(other);
}
