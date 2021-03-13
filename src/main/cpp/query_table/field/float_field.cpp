#include "float_field.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const FloatField &aValue) {
    return os << aValue.toString();
}


FloatField::FloatField(const Field &srcField) : Field(FieldType::FLOAT) {
        setValue(srcField.getValue<float_t>());
}

FloatField::FloatField(const FloatField &src)  : Field(src) { }

FloatField::FloatField(const float_t &src)  : Field(FieldType::FLOAT) {
    setValue(src);
}


FloatField::FloatField(const int8_t *src) : Field(Field::deserialize(FieldType::FLOAT, 0, src)) { }


// decrypt
FloatField::FloatField(const emp::Float &src, const int &party) : Field(FieldType::FLOAT){
    setValue((float_t) src.reveal<double>());
}

FloatField &FloatField::operator=(const FloatField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

BoolField FloatField::operator>=(const FloatField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField FloatField::operator==(const FloatField &cmp) const {

    // inspired by NoisePage: https://github.com/cmu-db/noisepage
        float_t right = cmp.getPayload();
        const double epsilon = std::fabs(right) * 0.01;
        return BoolField(std::fabs(getPayload() - right) <= epsilon);
    }


FloatField FloatField::select(const BoolField &choice, const FloatField &other) const {
    bool selection =  choice.getPayload();
    return selection ? FloatField(*this) :  FloatField(other);
}

