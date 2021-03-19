#include "float_instance.h"

using namespace vaultdb;

FloatInstance::FloatInstance(Field<BoolField> *src) : FieldInstance<BoolField>(src) {
    ref = static_cast<FloatField *>(src);
}


FloatInstance &FloatInstance::operator=(const FloatInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<BoolField> FloatInstance::plus(const Field<BoolField> &rhs) const {
    return *ref + static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::minus(const Field<BoolField> &rhs) const {
    return *ref - static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::multiply(const Field<BoolField> &rhs) const {
    return (*ref) * static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::div(const Field<BoolField> &rhs) const {
    return *ref / static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::modulus(const Field<BoolField> &rhs) const {
    return (*ref) % static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::select(const BoolField &choice, const Field<BoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<FloatField>(rhs));
}

Field<BoolField> FloatInstance::operator&(const Field<BoolField> &rhs) const {
    return (*ref) & static_cast<FloatField>(rhs);

}

Field<BoolField> FloatInstance::operator^(const Field<BoolField> &rhs) const {
    return (*ref) ^ static_cast<FloatField>(rhs);
}

Field<BoolField> FloatInstance::operator|(const Field<BoolField> &rhs) const {
    return (*ref) | static_cast<FloatField>(rhs);
}

BoolField FloatInstance::geq(const Field<BoolField> &rhs) const {
    return (*ref) >= static_cast<FloatField>(rhs);

}

BoolField FloatInstance::equal(const Field<BoolField> &rhs) const {
    return (*ref) == static_cast<FloatField>(rhs);

}

BoolField FloatInstance::negate() const {
    return ref->neg();
}

string FloatInstance::str() const {
    return ref->toString();
}

void FloatInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

