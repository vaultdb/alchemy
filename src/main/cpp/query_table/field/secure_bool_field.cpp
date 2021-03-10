#include "secure_bool_field.h"

vaultdb::SecureBoolField::SecureBoolField(const vaultdb::SecureBoolField &src) : Field(FieldType::SECURE_BOOL) {
    setValue(src.getPayload());
}



vaultdb::SecureBoolField::SecureBoolField(const Field *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_BOOL){
    bool bit = (myParty == dstParty) ? src->getValue<bool>() : 0;
    emp::Bit payload = emp::Bit(bit, dstParty);
    setValue<emp::Bit>(payload);
}

vaultdb::SecureBoolField::SecureBoolField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_BOOL, 0, src)){
}



vaultdb::SecureBoolField &vaultdb::SecureBoolField::operator=(const vaultdb::SecureBoolField &other)  {
    if(this == &other) return *this;
    copy(other);
    return *this;
}


std::ostream &vaultdb::operator<<(std::ostream &os, const SecureBoolField &aValue) {
    return os << aValue.toString();
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

vaultdb::SecureBoolField::SecureBoolField(const vaultdb::Field &srcField) : Field(FieldType::SECURE_BOOL) {
    setValue(srcField.getValue<emp::Bit>());

}

vaultdb::SecureBoolField::SecureBoolField(const emp::Bit &src) : Field(FieldType::SECURE_BOOL) {
    setValue(src);
}


