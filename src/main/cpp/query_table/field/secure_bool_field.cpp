#include "secure_bool_field.h"

vaultdb::SecureBoolField::SecureBoolField(const bool &src, const int &myParty, const int &dstParty) {
    bool bit = (myParty == dstParty) ? src : 0;
    emp::Bit payload = emp::Bit(bit, dstParty);
    field_ = Field::createSecureBool(payload);
    payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
}

vaultdb::SecureBoolField::SecureBoolField(const int8_t *src) {
    emp::Bit input = reinterpret_cast<const emp::Bit &>(*src);
    field_ = Field::createSecureBool(input);
    payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
}



vaultdb::SecureBoolField &vaultdb::SecureBoolField::operator=(const vaultdb::SecureBoolField &other) {
    if(this == &other) return *this;

    this->field_ = Field(other.getBaseField());
    this->payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
    return *this;
}


std::ostream &vaultdb::operator<<(std::ostream &os, const SecureBoolField &aValue) {
    return os << aValue.toString();
}

vaultdb::SecureBoolField vaultdb::SecureBoolField::operator==(const vaultdb::SecureBoolField &cmp) const {
    emp::Bit res = *payload_ == *cmp.payload_;
    return  SecureBoolField(res);
}

vaultdb::SecureBoolField
vaultdb::SecureBoolField::select(const vaultdb::SecureBoolField &choice, const vaultdb::SecureBoolField &other) const {
    emp::Bit selection = *(choice.payload_);
    emp::Bit result =  emp::If(selection, *payload_, *other.payload_);
    return  SecureBoolField(result);
}

vaultdb::SecureBoolField vaultdb::SecureBoolField::operator>=(const vaultdb::SecureBoolField &cmp) const {
    emp::Bit lhsVal = *payload_;
    emp::Bit rhsVal = *(cmp.payload_);
    emp::Bit gt = (lhsVal == emp::Bit(true)) & (rhsVal == emp::Bit(false));
    emp::Bit eq = lhsVal == rhsVal;

    return  SecureBoolField(eq | gt);


}


