#include "bool_instance.h"

using namespace vaultdb;

BoolInstance::BoolInstance(Field<BoolField> *src) : FieldInstance<BoolField>(src) {
    ref = static_cast<BoolField *>(src);
}


BoolInstance &BoolInstance::operator=(const BoolInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<BoolField> BoolInstance::plus(const Field<BoolField> &rhs) const {
    return *ref + static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::minus(const Field<BoolField> &rhs) const {
    return *ref - static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::multiply(const Field<BoolField> &rhs) const {
    return (*ref) * static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::div(const Field<BoolField> &rhs) const {
    return *ref / static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::modulus(const Field<BoolField> &rhs) const {
    return (*ref) % static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::select(const BoolField &choice, const Field<BoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<BoolField>(rhs));
}

Field<BoolField> BoolInstance::operator&(const Field<BoolField> &rhs) const {
    return (*ref) & static_cast<BoolField>(rhs);

}

Field<BoolField> BoolInstance::operator^(const Field<BoolField> &rhs) const {
    return (*ref) ^ static_cast<BoolField>(rhs);
}

Field<BoolField> BoolInstance::operator|(const Field<BoolField> &rhs) const {
    return (*ref) | static_cast<BoolField>(rhs);
}

BoolField BoolInstance::geq(const Field<BoolField> &rhs) const {
    return (*ref).gteq(static_cast<BoolField>(rhs));

}

BoolField BoolInstance::equal(const Field<BoolField> &rhs) const {
    return (*ref).eq(static_cast<BoolField>(rhs));
}

BoolField BoolInstance::negate() const {
    return ref->neg();
}

string BoolInstance::str() const {
    return ref->str();
}

void BoolInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

