#include "secure_bool_field.h"

using namespace vaultdb;

SecureBoolField::SecureBoolField(const SecureBoolField &src) : Field(FieldType::SECURE_BOOL) {
    emp::Bit p = src.getPayload();
    setValue(p);
}


SecureBoolField::SecureBoolField(const BoolField * src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_BOOL){
    bool bit = (myParty == dstParty) ? src->getValue<bool>() : false;
    emp::Bit payload = emp::Bit(bit, dstParty);
    setValue(payload);
}

SecureBoolField::SecureBoolField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_BOOL, 0, src)){
}



SecureBoolField &SecureBoolField::operator=(const SecureBoolField &other)  {
    if(this == &other) return *this;
    copy(other);
    return *this;
}


SecureBoolField::SecureBoolField(const bool &src) {

    emp::Bit s(src);
    setValue(s);

}


std::ostream &operator<<(std::ostream &os, const SecureBoolField &aValue) {
    return os << aValue.toString();
}

SecureBoolField SecureBoolField::eq(const SecureBoolField &cmp) const {
    emp::Bit res = (getPayload() == cmp.getPayload());
    return  SecureBoolField(res);
}

SecureBoolField
SecureBoolField::selectValue(const SecureBoolField &choice, const SecureBoolField &other) const {
    emp::Bit selection = choice.getPayload();
    emp::Bit result =  emp::If(selection, getPayload(), other.getPayload());
    return  SecureBoolField(result);
}

SecureBoolField SecureBoolField::gteq(const SecureBoolField &cmp) const {
    emp::Bit lhsVal = getPayload();
    emp::Bit rhsVal = cmp.getPayload();
    emp::Bit gt = (lhsVal == emp::Bit(true)) & (rhsVal == emp::Bit(false));
    emp::Bit eq = lhsVal == rhsVal;

    return  SecureBoolField(eq | gt);


}

SecureBoolField::SecureBoolField(const Field &srcField) : Field(FieldType::SECURE_BOOL) {
    setValue(srcField.getValue<emp::Bit>());

}

SecureBoolField::SecureBoolField(const emp::Bit &src) : Field(FieldType::SECURE_BOOL) {
    setValue(src);
}

void SecureBoolField::ser(int8_t *target) const {
    emp::Bit bit = getPayload();
    memcpy(target, (int8_t *) &(bit.bit), allocated_size_);
}



