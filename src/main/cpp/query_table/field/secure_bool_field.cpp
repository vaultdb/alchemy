#include "secure_bool_field.h"

vaultdb::SecureBoolField::SecureBoolField(const vaultdb::SecureBoolField &src) {
    field_ = Field::createSecureBool(src.getPayload());
}



vaultdb::SecureBoolField::SecureBoolField(const bool &src, const int &myParty, const int &dstParty) {
    bool bit = (myParty == dstParty) ? src : 0;
    emp::Bit payload = emp::Bit(bit, dstParty);
    field_ = Field::createSecureBool(payload);
}

vaultdb::SecureBoolField::SecureBoolField(const int8_t *src) {
    field_ = Field::deserialize(FieldType::SECURE_BOOL, 0, src);
}



vaultdb::SecureBoolField &vaultdb::SecureBoolField::operator=(const vaultdb::SecureBoolField &other) {
    if(this == &other) return *this;

    this->field_ = Field(other.getBaseField());
    return *this;
}


std::ostream &vaultdb::operator<<(std::ostream &os, const SecureBoolField &aValue) {
    return os << aValue.getBaseField();
}

vaultdb::SecureBoolField vaultdb::SecureBoolField::operator==(const vaultdb::SecureBoolField &cmp) const {
    emp::Bit res = (getPayload() == cmp.getPayload());
    return  SecureBoolField(res);
}

vaultdb::SecureBoolField
vaultdb::SecureBoolField::select(const vaultdb::SecureBoolField &choice, const vaultdb::SecureBoolField &other) const {
    emp::Bit selection = choice.getPayload();
    emp::Bit result =  emp::If(selection, getPayload(), other.getPayload());
    return  SecureBoolField(result);
}

vaultdb::SecureBoolField vaultdb::SecureBoolField::operator>=(const vaultdb::SecureBoolField &cmp) const {
    emp::Bit lhsVal = getPayload();
    emp::Bit rhsVal = cmp.getPayload();
    emp::Bit gt = (lhsVal == emp::Bit(true)) & (rhsVal == emp::Bit(false));
    emp::Bit eq = lhsVal == rhsVal;

    return  SecureBoolField(eq | gt);


}


