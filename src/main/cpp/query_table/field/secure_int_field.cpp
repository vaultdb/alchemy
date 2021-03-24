#include "secure_int_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureIntField &aValue) {
    return os << aValue.toString();
}


SecureIntField::SecureIntField(const Field &srcField) : Field(FieldType::SECURE_INT) {
    emp::Integer src = srcField.getValue<emp::Integer>();
    setValue(src);
}

SecureIntField::SecureIntField(const int32_t &srcField) : Field(FieldType::SECURE_INT) {
    emp::Integer src(32, srcField);
    setValue(src);
}


SecureIntField::SecureIntField(const SecureIntField &src) : Field(src) { }



SecureIntField::SecureIntField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_INT, 0, src)) { }

SecureIntField::SecureIntField(const IntField *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_INT){
    int32_t toEncrypt = (myParty == dstParty) ? src->getValue<int32_t>() : 0;
    emp::Integer payload = emp::Integer(32, toEncrypt, dstParty);
    *((emp::Integer *) data_) = emp::Integer(payload);
//    setValue(payload);
}



SecureIntField::SecureIntField(const emp::Integer &src) : Field(FieldType::SECURE_INT) {
    setValue(src);
}


SecureIntField &SecureIntField::operator=(const SecureIntField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

SecureBoolField SecureIntField::operator>=(const SecureIntField &cmp) const {
    emp::Bit res =  (getPayload() >= cmp.getPayload());
    return SecureBoolField(res);
}

SecureBoolField SecureIntField::operator==(const SecureIntField &cmp) const {

    return  SecureBoolField(getPayload() == cmp.getPayload());
}

SecureIntField SecureIntField::selectValue(const SecureBoolField &choice, const SecureIntField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Integer res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureIntField(res);
}

emp::Integer SecureIntField::getPayload() const {
    emp::Integer res = getValue<emp::Integer>();
    assert(res.size() == 32);
    return res;
}







