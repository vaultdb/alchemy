#include "long_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const LongField &aValue) {
    return os << aValue.toString();
}


LongField::LongField(const LongField &src) : Field(src){ }

LongField::LongField(const Field &srcField) : Field(FieldType::LONG) {
    auto v = srcField.getValue<int64_t>();
    setValue(v);
}

LongField::LongField(const int64_t &src)  : Field(FieldType::LONG){
    setValue(src);

}


LongField::LongField(const int8_t *src) : Field(FieldType::LONG) {
        *((int64_t *) data_) =  *((int64_t *) src);
}


LongField::LongField(const emp::Integer &src, const int &party) : Field(FieldType::LONG) {
    int64_t revealed = src.reveal<int64_t>(party);
    setValue(revealed);
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

LongField LongField::selectValue(const BoolField &choice, const LongField &other) const {
    bool selection =  choice.getPayload();
    return selection ? LongField(*this) :  LongField(other);
}



