#include "secure_long_instance.h"

using namespace vaultdb;

SecureLongInstance::SecureLongInstance(Field<SecureBoolField> *src) : FieldInstance<SecureBoolField>(src) {
    ref = static_cast<SecureLongField *>(src);
}


SecureLongInstance &SecureLongInstance::operator=(const SecureLongInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<SecureBoolField> SecureLongInstance::plus(const Field<SecureBoolField> &rhs) const {
    return *ref + static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::minus(const Field<SecureBoolField> &rhs) const {
    return *ref - static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::multiply(const Field<SecureBoolField> &rhs) const {
    return (*ref) * static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::div(const Field<SecureBoolField> &rhs) const {
    return *ref / static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::modulus(const Field<SecureBoolField> &rhs) const {
    return (*ref) % static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::select(const SecureBoolField &choice, const Field<SecureBoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<SecureLongField>(rhs));
}

Field<SecureBoolField> SecureLongInstance::operator&(const Field<SecureBoolField> &rhs) const {
    return (*ref) & static_cast<SecureLongField>(rhs);

}

Field<SecureBoolField> SecureLongInstance::operator^(const Field<SecureBoolField> &rhs) const {
    return (*ref) ^ static_cast<SecureLongField>(rhs);
}

Field<SecureBoolField> SecureLongInstance::operator|(const Field<SecureBoolField> &rhs) const {
    return (*ref) | static_cast<SecureLongField>(rhs);
}

SecureBoolField SecureLongInstance::geq(const Field<SecureBoolField> &rhs) const {
    return (*ref) >= static_cast<SecureLongField>(rhs);

}

SecureBoolField SecureLongInstance::equal(const Field<SecureBoolField> &rhs) const {
    return (*ref) == static_cast<SecureLongField>(rhs);

}

SecureBoolField SecureLongInstance::negate() const {
    return ref->neg();
}

string SecureLongInstance::str() const {
    return ref->toString();
}

void SecureLongInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

