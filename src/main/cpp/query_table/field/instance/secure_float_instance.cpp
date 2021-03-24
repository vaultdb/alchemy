#include "secure_float_instance.h"

using namespace vaultdb;

SecureFloatInstance::SecureFloatInstance(Field<SecureBoolField> *src) : FieldInstance<SecureBoolField>(src) {
    ref = static_cast<SecureFloatField *>(src);
}


SecureFloatInstance &SecureFloatInstance::operator=(const SecureFloatInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<SecureBoolField> SecureFloatInstance::plus(const Field<SecureBoolField> &rhs) const {
    return *ref + static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::minus(const Field<SecureBoolField> &rhs) const {
    return *ref - static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::multiply(const Field<SecureBoolField> &rhs) const {
    return (*ref) * static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::div(const Field<SecureBoolField> &rhs) const {
    return *ref / static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::modulus(const Field<SecureBoolField> &rhs) const {
    return (*ref) % static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::select(const SecureBoolField &choice, const Field<SecureBoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<SecureFloatField>(rhs));
}

Field<SecureBoolField> SecureFloatInstance::operator&(const Field<SecureBoolField> &rhs) const {
    return (*ref) & static_cast<SecureFloatField>(rhs);

}

Field<SecureBoolField> SecureFloatInstance::operator^(const Field<SecureBoolField> &rhs) const {
    return (*ref) ^ static_cast<SecureFloatField>(rhs);
}

Field<SecureBoolField> SecureFloatInstance::operator|(const Field<SecureBoolField> &rhs) const {
    return (*ref) | static_cast<SecureFloatField>(rhs);
}

SecureBoolField SecureFloatInstance::geq(const Field<SecureBoolField> &rhs) const {
    return (*ref) >= static_cast<SecureFloatField>(rhs);

}

SecureBoolField SecureFloatInstance::equal(const Field<SecureBoolField> &rhs) const {
    return (*ref) == static_cast<SecureFloatField>(rhs);

}

SecureBoolField SecureFloatInstance::negate() const {
    return ref->neg();
}

string SecureFloatInstance::str() const {
    return ref->str();
}

void SecureFloatInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

