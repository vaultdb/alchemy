#include "int_field.h"
#include "bool_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const IntField &aValue) {
    return os << aValue.toString();
}



IntField::IntField(const Field &srcField) : Field(FieldType::INT) {
    int32_t p = srcField.getValue<int32_t>();
    setValue(p);


}

IntField::IntField(const IntField &src) : Field(src) { }

IntField::IntField(const int32_t &src) : Field(FieldType::INT){

    setValue(src);
}


IntField::IntField(const int8_t *src) :Field(Field::deserialize(FieldType::INT, 0, src)) { }


IntField::IntField(const emp::Integer &src, const int &party) : Field(FieldType::INT){
    int32_t revealed = src.reveal<int32_t>(party);
    setValue(revealed);
}

IntField &IntField::operator=(const IntField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

IntField &IntField::operator=(const int32_t &other) {
    setValue(other);
    return *this;
}


BoolField IntField::operator>=(const IntField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField IntField::operator==(const IntField &cmp) const {
    bool res = (getPayload()) == (cmp.getPayload());
    return  BoolField(res);
}

IntField IntField::select(const BoolField &choice, const IntField &other) const {
    bool selection =  choice.getPayload();
    return selection ? IntField(*this) :  IntField(other);
}



