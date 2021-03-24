#include "secure_int_field.h"
#include <util/type_utilities.h>

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



SecureIntField::SecureIntField(const int8_t *src) : Field(FieldType::SECURE_INT) {
    emp::Bit *srcPtr = (emp::Bit *) src;
    emp::Integer v(32, 0, emp::PUBLIC);
    memcpy(v.bits.data(), srcPtr, 32*sizeof(emp::Bit));
    *((emp::Integer *) data_) = v;
}

SecureIntField::SecureIntField(const IntField *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_INT){
    int32_t toEncrypt = (myParty == dstParty) ? src->getValue<int32_t>() : 0;
    emp::Integer payload = emp::Integer(32, toEncrypt, dstParty);
    *((emp::Integer *) data_) = emp::Integer(payload);
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

void SecureIntField::ser(int8_t *target) const {
    size_t len = TypeUtilities::getTypeSize(type_)/8;
    memcpy(target, (int8_t *) getPayload().bits.data(), len);
}







