#include "secure_int_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureIntField &aValue) {
    return os << aValue.getBaseField();
}


SecureIntField::SecureIntField(const Field &srcField) : field_(srcField) { }

SecureIntField::SecureIntField(const SecureIntField &src) {
    field_ =  Field(src.getBaseField());
}



SecureIntField::SecureIntField(const int8_t *src) {
    field_ = Field::deserialize(FieldType::SECURE_INT32, 0, src);
}

SecureIntField::SecureIntField(const int32_t &src, const int &myParty, const int &dstParty) {
    int32_t toEncrypt = (myParty == dstParty) ? src : 0;
    emp::Integer payload = emp::Integer(32, toEncrypt, dstParty);
    field_ = Field::createSecureInt32(payload);
}



SecureIntField::SecureIntField(const emp::Integer &src) {
    field_ = Field::createSecureInt32(src);
}


SecureIntField &SecureIntField::operator=(const SecureIntField &other) {
    if(this == &other) return *this;
    this->field_ = Field(other.getBaseField());
    return *this;
}

SecureBoolField SecureIntField::operator>=(const SecureIntField &cmp) const {
    emp::Bit res =  (getPayload() >= cmp.getPayload());
    return SecureBoolField(res);
}

SecureBoolField SecureIntField::operator==(const SecureIntField &cmp) const {
    emp::Bit res = ((getPayload()) == (cmp.getPayload()));
    return  SecureBoolField(res);
}

SecureIntField SecureIntField::select(const SecureBoolField &choice, const SecureIntField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Integer res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureIntField(res);
}



