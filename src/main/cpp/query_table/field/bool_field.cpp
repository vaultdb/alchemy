#include "bool_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const BoolField &aValue) {
    return os << aValue.toString();
}

BoolField::BoolField(const Field<BoolField> &srcField) : Field(FieldType::BOOL) {
    bool payload = srcField.getValue<bool>();
    setValue(payload);
}

BoolField::BoolField(const BoolField &src) :Field(src) {
}

BoolField::BoolField(const bool &src) : Field(FieldType::BOOL){
    setValue(src);
}

BoolField::BoolField(const emp::Bit &src, const int &party)
    : Field(FieldType::BOOL){

    setValue(src.reveal(party));
}

BoolField::BoolField(const int8_t *src) : Field(Field::deserialize(FieldType::BOOL, 0, src)){ }


BoolField &BoolField::operator=(const BoolField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

BoolField BoolField::gteq(const BoolField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField BoolField::eq(const BoolField &cmp) const {
    bool res = (getPayload()) == (cmp.getPayload());
    return  BoolField(res);
}

BoolField BoolField::selectValue(const BoolField &choice, const BoolField &other) const {
    bool selection =  choice.getPayload();
    return selection ? BoolField(*this) :  BoolField(other);
}

BoolField BoolField::neg() const {
    return BoolField(!(getPayload()));
}

