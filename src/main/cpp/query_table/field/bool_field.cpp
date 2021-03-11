#include "bool_field.h"

std::ostream &vaultdb::operator<<(std::ostream &os, const vaultdb::BoolField &aValue) {
    return os << aValue.toString();
}

vaultdb::BoolField::BoolField(const vaultdb::Field &srcField) : Field(FieldType::BOOL) {
    bool payload = srcField.getValue<bool>();
    setValue(payload);
}

vaultdb::BoolField::BoolField(const vaultdb::BoolField &src) :Field(src) {
}

vaultdb::BoolField::BoolField(const bool &src) : Field(FieldType::BOOL){
    setValue(src);
}

vaultdb::BoolField::BoolField(const emp::Bit &src, const int &party)
    : Field(FieldType::BOOL){

    setValue(src.reveal(party));
}

vaultdb::BoolField::BoolField(const int8_t *src) : Field(Field::deserialize(FieldType::BOOL, 0, src)){ }


vaultdb::BoolField &vaultdb::BoolField::operator=(const vaultdb::BoolField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

vaultdb::BoolField vaultdb::BoolField::operator>=(const vaultdb::BoolField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::operator==(const vaultdb::BoolField &cmp) const {
    bool res = (getPayload()) == (cmp.getPayload());
    return  BoolField(res);
}

vaultdb::BoolField vaultdb::BoolField::select(const vaultdb::BoolField &choice, const vaultdb::BoolField &other) const {
    bool selection =  choice.getPayload();
    return selection ? BoolField(*this) :  BoolField(other);
}

