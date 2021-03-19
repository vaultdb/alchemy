#include "long_instance.h"

using namespace vaultdb;

LongInstance::LongInstance(Field<BoolField> *src) : FieldInstance<BoolField>(src) {
    ref = static_cast<LongField *>(src);
}


LongInstance &LongInstance::operator=(const LongInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<BoolField> LongInstance::plus(const Field<BoolField> &rhs) const {
    return *ref + static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::minus(const Field<BoolField> &rhs) const {
    return *ref - static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::multiply(const Field<BoolField> &rhs) const {
    return (*ref) * static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::div(const Field<BoolField> &rhs) const {
    return *ref / static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::modulus(const Field<BoolField> &rhs) const {
    return (*ref) % static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::select(const BoolField &choice, const Field<BoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<LongField>(rhs));
}

Field<BoolField> LongInstance::operator&(const Field<BoolField> &rhs) const {
    return (*ref) & static_cast<LongField>(rhs);

}

Field<BoolField> LongInstance::operator^(const Field<BoolField> &rhs) const {
    return (*ref) ^ static_cast<LongField>(rhs);
}

Field<BoolField> LongInstance::operator|(const Field<BoolField> &rhs) const {
    return (*ref) | static_cast<LongField>(rhs);
}

BoolField LongInstance::geq(const Field<BoolField> &rhs) const {
    return (*ref) >= static_cast<LongField>(rhs);

}

BoolField LongInstance::equal(const Field<BoolField> &rhs) const {
    return (*ref) == static_cast<LongField>(rhs);

}

BoolField LongInstance::negate() const {
    return ref->neg();
}

string LongInstance::str() const {
    return ref->str();
}

void LongInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

