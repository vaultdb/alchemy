#include "secure_float_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureFloatField &aValue) {
    return os << aValue.toString();
}


SecureFloatField::SecureFloatField(const Field &srcField) : Field(FieldType::SECURE_FLOAT) {
    setValue(srcField.getValue<emp::Float>());

}

SecureFloatField::SecureFloatField(const SecureFloatField &src) : Field(src) { }



SecureFloatField::SecureFloatField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_INT, 0, src)){ }

SecureFloatField::SecureFloatField(const FloatField *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_FLOAT){
    float_t toEncrypt = (myParty == dstParty) ? src->getValue<float_t>() : 0;
    emp::Float payload = emp::Float(toEncrypt, dstParty);
    setValue(payload);
}



SecureFloatField::SecureFloatField(const emp::Float &src) : Field(FieldType::SECURE_FLOAT) {
    setValue(src);
}


SecureFloatField &SecureFloatField::operator=(const SecureFloatField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

SecureBoolField SecureFloatField::operator>=(const SecureFloatField &cmp) const {
    emp::Float lhs = getPayload();
    emp::Float rhs = cmp.getPayload();
    emp::Bit res =  !(lhs.less_than(rhs));
    return SecureBoolField(res);
}

SecureBoolField SecureFloatField::operator==(const SecureFloatField &cmp) const {
    emp::Bit res = ((getPayload()).equal(cmp.getPayload()));
    return  SecureBoolField(res);
}

SecureFloatField SecureFloatField::selectValue(const SecureBoolField &choice, const SecureFloatField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Float res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureFloatField(res);
}

SecureFloatField SecureFloatField::operator&(const SecureFloatField &right) const {
    return SecureFloatField((getPayload()) & right.getPayload());
}

SecureFloatField SecureFloatField::operator^(const SecureFloatField &right) const {
    return SecureFloatField((getPayload()) ^ (right.getPayload()));
}

SecureFloatField SecureFloatField::operator|(const SecureFloatField &right) const {
    throw;
}



vaultdb::SecureFloatField vaultdb::SecureFloatField::operator+(const vaultdb::SecureFloatField &rhs) const {
    return SecureFloatField(getPayload() + rhs.getPayload());
}

void SecureFloatField::ser(int8_t *target) const {
    memcpy(target, (int8_t *) getPayload().value.data(), allocated_size_);
}
