#include "int_instance.h"

using namespace vaultdb;

IntInstance::IntInstance(Field<BoolField> *src) : FieldInstance<BoolField>(src) {
    ref = static_cast<IntField *>(src);
}


IntInstance &IntInstance::operator=(const IntInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<BoolField> IntInstance::plus(const Field<BoolField> &rhs) const {
    return *ref + static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::minus(const Field<BoolField> &rhs) const {
    return *ref - static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::multiply(const Field<BoolField> &rhs) const {
    return (*ref) * static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::div(const Field<BoolField> &rhs) const {
    return *ref / static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::modulus(const Field<BoolField> &rhs) const {
    return (*ref) % static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::select(const BoolField &choice, const Field<BoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<IntField>(rhs));
}

Field<BoolField> IntInstance::operator&(const Field<BoolField> &rhs) const {
    return (*ref) & static_cast<IntField>(rhs);

}

Field<BoolField> IntInstance::operator^(const Field<BoolField> &rhs) const {
    return (*ref) ^ static_cast<IntField>(rhs);
}

Field<BoolField> IntInstance::operator|(const Field<BoolField> &rhs) const {
    return (*ref) | static_cast<IntField>(rhs);
}

BoolField IntInstance::geq(const Field<BoolField> &rhs) const {
    return (*ref) >= static_cast<IntField>(rhs);

}

BoolField IntInstance::equal(const Field<BoolField> &rhs) const {
    return (*ref) == static_cast<IntField>(rhs);

}

BoolField IntInstance::negate() const {
    return ref->neg();
}

string IntInstance::str() const {
    return ref->toString();
}

void IntInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

