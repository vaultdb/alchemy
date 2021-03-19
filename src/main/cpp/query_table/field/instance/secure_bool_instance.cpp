#include "secure_bool_instance.h"

using namespace vaultdb;

SecureBoolInstance::SecureBoolInstance(Field<SecureBoolField> *src) : FieldInstance<SecureBoolField>(src) {
    ref = static_cast<SecureBoolField *>(src);
}


SecureBoolInstance &SecureBoolInstance::operator=(const SecureBoolInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<SecureBoolField> SecureBoolInstance::plus(const Field<SecureBoolField> &rhs) const {
    return *ref + static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::minus(const Field<SecureBoolField> &rhs) const {
    return *ref - static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::multiply(const Field<SecureBoolField> &rhs) const {
    return (*ref) * static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::div(const Field<SecureBoolField> &rhs) const {
    return *ref / static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::modulus(const Field<SecureBoolField> &rhs) const {
    return (*ref) % static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::select(const SecureBoolField &choice, const Field<SecureBoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<SecureBoolField>(rhs));
}

Field<SecureBoolField> SecureBoolInstance::operator&(const Field<SecureBoolField> &rhs) const {
    return (*ref) & static_cast<SecureBoolField>(rhs);

}

Field<SecureBoolField> SecureBoolInstance::operator^(const Field<SecureBoolField> &rhs) const {
    return (*ref) ^ static_cast<SecureBoolField>(rhs);
}

Field<SecureBoolField> SecureBoolInstance::operator|(const Field<SecureBoolField> &rhs) const {
    return (*ref) | static_cast<SecureBoolField>(rhs);
}

SecureBoolField SecureBoolInstance::geq(const Field<SecureBoolField> &rhs) const {
    return (*ref) >= static_cast<SecureBoolField>(rhs);

}

SecureBoolField SecureBoolInstance::equal(const Field<SecureBoolField> &rhs) const {
    return (*ref).eq(static_cast<SecureBoolField>(rhs));

}

SecureBoolField SecureBoolInstance::negate() const {
    return ref->neg();
}

string SecureBoolInstance::str() const {
    return ref->toString();
}

void SecureBoolInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

