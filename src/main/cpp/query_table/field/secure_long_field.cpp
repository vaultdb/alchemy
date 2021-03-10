#include "secure_long_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureLongField &aValue) {
    return os << aValue.toString();
}


SecureLongField::SecureLongField(const Field &srcField) : Field(FieldType::SECURE_LONG) {
    setValue(srcField.getValue<emp::Integer>());

}

SecureLongField::SecureLongField(const SecureLongField &src) : Field(src) { }



SecureLongField::SecureLongField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_LONG, 0, src)) { }

SecureLongField::SecureLongField(const Field *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_LONG) {
    int64_t toEncrypt = (myParty == dstParty) ? src->getValue<int64_t>() : 0;
    emp::Integer payload = emp::Integer(64, toEncrypt, dstParty);
    setValue<emp::Integer>(payload);
}



SecureLongField::SecureLongField(const emp::Integer &src) : Field(FieldType::SECURE_LONG){
    setValue(src);
}


SecureLongField &SecureLongField::operator=(const SecureLongField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

SecureBoolField SecureLongField::operator>=(const SecureLongField &cmp) const {
    emp::Bit res =  (getPayload() >= cmp.getPayload());
    return SecureBoolField(res);
}

SecureBoolField SecureLongField::operator==(const SecureLongField &cmp) const {
    emp::Bit res = ((getPayload()) == (cmp.getPayload()));
    return  SecureBoolField(res);
}

SecureLongField SecureLongField::select(const SecureBoolField &choice, const SecureLongField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Integer res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureLongField(res);
}

SecureLongField SecureLongField::operator&(const SecureLongField &right) const {
    return SecureLongField((getPayload()) & right.getPayload());
}

SecureLongField SecureLongField::operator^(const SecureLongField &right) const {
    return SecureLongField((getPayload()) ^ (right.getPayload()));
}

SecureLongField SecureLongField::operator|(const SecureLongField &right) const {
    return SecureLongField((getPayload()) | (right.getPayload()));
}

emp::Integer SecureLongField::getPayload() const {
    emp::Integer res = getValue<emp::Integer>();
    assert(res.size() == 64);
    return res;
}

