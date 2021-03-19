#include "secure_string_instance.h"

using namespace vaultdb;

SecureStringInstance::SecureStringInstance(Field<SecureBoolField> *src) : FieldInstance<SecureBoolField>(src) {
    ref = static_cast<SecureStringField *>(src);
}


SecureStringInstance &SecureStringInstance::operator=(const SecureStringInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<SecureBoolField> SecureStringInstance::plus(const Field<SecureBoolField> &rhs) const {
    return *ref + static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::minus(const Field<SecureBoolField> &rhs) const {
    return *ref - static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::multiply(const Field<SecureBoolField> &rhs) const {
    return (*ref) * static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::div(const Field<SecureBoolField> &rhs) const {
    return *ref / static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::modulus(const Field<SecureBoolField> &rhs) const {
    return (*ref) % static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::select(const SecureBoolField &choice, const Field<SecureBoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<SecureStringField>(rhs));
}

Field<SecureBoolField> SecureStringInstance::operator&(const Field<SecureBoolField> &rhs) const {
    return (*ref) & static_cast<SecureStringField>(rhs);

}

Field<SecureBoolField> SecureStringInstance::operator^(const Field<SecureBoolField> &rhs) const {
    return (*ref) ^ static_cast<SecureStringField>(rhs);
}

Field<SecureBoolField> SecureStringInstance::operator|(const Field<SecureBoolField> &rhs) const {
    return (*ref) | static_cast<SecureStringField>(rhs);
}

SecureBoolField SecureStringInstance::geq(const Field<SecureBoolField> &rhs) const {
    return (*ref) >= static_cast<SecureStringField>(rhs);

}

SecureBoolField SecureStringInstance::equal(const Field<SecureBoolField> &rhs) const {
    return (*ref) == static_cast<SecureStringField>(rhs);

}

SecureBoolField SecureStringInstance::negate() const {
    return ref->neg();
}

string SecureStringInstance::str() const {
    return ref->toString();
}

void SecureStringInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

