#include "bool_field.h"

std::ostream &vaultdb::operator<<(std::ostream &os, const vaultdb::BoolField &aValue) {
    return os << aValue.getBaseField();
}

vaultdb::BoolField::BoolField(const vaultdb::Field &srcField) : field_(srcField) { }

vaultdb::BoolField::BoolField(const vaultdb::BoolField &src) {
    field_ =  Field(src.getBaseField());
}

vaultdb::BoolField::BoolField(const bool &src) {
    field_ = Field::createBool(src);
}

vaultdb::BoolField::BoolField(const emp::Bit &src, const int &party) {
    bool revealed = src.reveal(party);
    field_ = Field::createBool(revealed);
}

vaultdb::BoolField &vaultdb::BoolField::operator=(const vaultdb::BoolField &other) {
    if(this == &other) return *this;
    this->field_ = Field(other.getBaseField());
    return *this;
}

vaultdb::BoolField vaultdb::BoolField::operator>=(const vaultdb::BoolField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::operator==(const vaultdb::BoolField &cmp) const {
    bool res = (field_.getValue<bool>()) == (cmp.getBaseField().getValue<bool>());
    return  BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::select(const vaultdb::BoolField &choice, const vaultdb::BoolField &other) const {
    bool selection =  choice.getPayload();
    return selection ? BoolField(*this) :  BoolField(other);
}

vaultdb::BoolField::BoolField(const int8_t *src) {
    field_ = Field::deserialize(FieldType::BOOL, 0, src);
}
