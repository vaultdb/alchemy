#include "long_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const LongField &aValue) {
    return os << aValue.toString();
}


LongField::LongField(const LongField &src) : Field(src){ }

LongField::LongField(const Field &srcField) : Field(FieldType::LONG) {
    setValue(srcField.getValue<int64_t>());
}

LongField::LongField(const int64_t &src)  : Field(FieldType::LONG){
    setValue(src);
}


LongField::LongField(const int8_t *src) : Field(Field::deserialize(FieldType::LONG, 0, src)) { }


LongField::LongField(const emp::Integer &src, const int &party) : Field(FieldType::LONG) {
    int64_t revealed = src.reveal<int64_t>(party);
    setValue<int64_t> (revealed);
}

LongField &LongField::operator=(const LongField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

BoolField LongField::operator>=(const LongField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField LongField::operator==(const LongField &cmp) const {
    bool res = (getPayload()) == (cmp.getPayload());
    return  BoolField(res);
}

LongField LongField::select(const BoolField &choice, const LongField &other) const {
    bool selection =  choice.getPayload();
    return selection ? LongField(*this) :  LongField(other);
}



