#include "string_instance.h"

using namespace vaultdb;

StringInstance::StringInstance(Field<BoolField> *src) : FieldInstance<BoolField>(src) {
    ref = static_cast<StringField *>(src);
}


StringInstance &StringInstance::operator=(const StringInstance &other) {
    if(&other == this)
        return *this;

    // it points to the same thing as before
    this->ref = other.ref;
    return *this;

}


Field<BoolField> StringInstance::plus(const Field<BoolField> &rhs) const {
    return *ref + static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::minus(const Field<BoolField> &rhs) const {
    return *ref - static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::multiply(const Field<BoolField> &rhs) const {
    return (*ref) * static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::div(const Field<BoolField> &rhs) const {
    return *ref / static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::modulus(const Field<BoolField> &rhs) const {
    return (*ref) % static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::select(const BoolField &choice, const Field<BoolField> &rhs) const {
    return ref->selectValue(choice, static_cast<StringField>(rhs));
}

Field<BoolField> StringInstance::operator&(const Field<BoolField> &rhs) const {
    return (*ref) & static_cast<StringField>(rhs);

}

Field<BoolField> StringInstance::operator^(const Field<BoolField> &rhs) const {
    return (*ref) ^ static_cast<StringField>(rhs);
}

Field<BoolField> StringInstance::operator|(const Field<BoolField> &rhs) const {
    return (*ref) | static_cast<StringField>(rhs);
}

BoolField StringInstance::geq(const Field<BoolField> &rhs) const {
    return (*ref) >= static_cast<StringField>(rhs);

}

BoolField StringInstance::equal(const Field<BoolField> &rhs) const {
    return (*ref) == static_cast<StringField>(rhs);

}

BoolField StringInstance::negate() const {
    return ref->neg();
}

string StringInstance::str() const {
    return ref->str();
}

void StringInstance::serialize(int8_t *dst) const {
    ref->ser(dst);
}

