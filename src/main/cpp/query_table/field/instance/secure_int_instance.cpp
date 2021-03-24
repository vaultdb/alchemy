#include "secure_int_instance.h"

using namespace vaultdb;

SecureIntInstance::SecureIntInstance(Field<SecureBoolField> *src) : FieldInstance<SecureBoolField>(src) {
    ref = static_cast<SecureIntField *>(src);
}


SecureIntInstance &SecureIntInstance::operator=(const SecureIntInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<SecureBoolField> SecureIntInstance::plus(const Field<SecureBoolField> &rhs) const {
    return *ref + static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::minus(const Field<SecureBoolField> &rhs) const {
    return *ref - static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::multiply(const Field<SecureBoolField> &rhs) const {
    return (*ref) * static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::div(const Field<SecureBoolField> &rhs) const {
    return *ref / static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::modulus(const Field<SecureBoolField> &rhs) const {
    return (*ref) % static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::select(const SecureBoolField &choice, const Field<SecureBoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<SecureIntField>(rhs));
}

Field<SecureBoolField> SecureIntInstance::operator&(const Field<SecureBoolField> &rhs) const {
    return (*ref) & static_cast<SecureIntField>(rhs);

}

Field<SecureBoolField> SecureIntInstance::operator^(const Field<SecureBoolField> &rhs) const {
    return (*ref) ^ static_cast<SecureIntField>(rhs);
}

Field<SecureBoolField> SecureIntInstance::operator|(const Field<SecureBoolField> &rhs) const {
    return (*ref) | static_cast<SecureIntField>(rhs);
}

SecureBoolField SecureIntInstance::geq(const Field<SecureBoolField> &rhs) const {
    return (*ref) >= static_cast<SecureIntField>(rhs);

}

SecureBoolField SecureIntInstance::equal(const Field<SecureBoolField> &rhs) const {
    return (*ref) == static_cast<SecureIntField>(rhs);

}

SecureBoolField SecureIntInstance::negate() const {
    return ref->neg();
}

string SecureIntInstance::str() const {
    return ref->str();
}

void SecureIntInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

