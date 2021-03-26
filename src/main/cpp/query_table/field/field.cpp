#include "field.h"
#include "util/utilities.h"
#include "field_factory.h"
#include <query_table/field/visitors.h>
#include <util/field_utilities.h>
#include <util/type_utilities.h>

using namespace vaultdb;


std::ostream &vaultdb::operator<<(std::ostream &os, const Field<bool> &aValue) {
    return os << aValue.toString();
}

std::ostream &vaultdb::operator<<(std::ostream &os, const Field<emp::Bit> &aValue) {
    return os << aValue.toString();
}

template<typename B>
Field<B>::Field() :  payload_(false), type_(FieldType::INVALID), string_length_(0) {
    assert(FieldUtilities::validateTypeAlignment(*this));
}


/*template<typename B>
Field<B>::Field(const FieldType &typeId, const int &strLength) : type_(typeId), string_length_(strLength) {
    assert(FieldUtilities::validateTypeAlignment(*this));
}*/

template<typename B>
Field<B>::Field(const FieldType &fieldType, const Value &val, const int &strLength) : payload_(val), type_(fieldType), string_length_(strLength) {
    assert(FieldUtilities::validateTypeAlignment(*this));

}


template<typename B>
Field<B>::Field(const Field<B> &field) :  payload_(field.payload_), type_(field.type_), string_length_(field.string_length_) {
    assert(FieldUtilities::validateTypeAlignment(*this));

}

template<typename B>
Field<B>::Field(const B &value) : string_length_(0) {
    payload_ = value;
    type_ = (std::is_same_v<bool, B>)  ? FieldType::BOOL : FieldType::SECURE_BOOL;
}


template<typename B>
Field<B> &Field<B>::operator=(const Field<B> &other) {

    if(type_ == FieldType::INVALID && other.type_ == FieldType::INVALID) return *this; // nothing to do here
    assert(other.type_ != FieldType::INVALID);

    if(&other == this)
        return *this;

    payload_ = other.payload_;
    string_length_ = other.string_length_;
    type_ = other.type_;
    return *this;

}

template<typename B>
FieldType Field<B>::getType() const {
    return type_;
}

template<typename B>
size_t Field<B>::getSize() const {

    // serialization size
    return FieldUtilities::getPhysicalSize(type_, string_length_);
}






template<typename B>
B Field<B>::operator==(const Field<B> &cmp) const {
    if(type_ != cmp.type_) return B(false);
    if(getSize() != cmp.getSize()) return B(false);

    EqualityVisitor visitor;
    visitor.rhs = cmp.payload_;
    return boost::get<B>(boost::apply_visitor(visitor, payload_));
}

template<typename B>
B Field<B>::operator!=(const Field &cmp) const {
    B eq = *this == cmp;
    return !eq;
}

// only applicable to bool types
template<typename B>
B Field<B>::operator!() const {
    return !(boost::get<B>(payload_));
}

template<typename B>
B Field<B>::operator>=(const Field &r) const {
    GreaterThanEqVisitor visitor;
    visitor.rhs = r.payload_;
    return boost::get<B>(boost::apply_visitor(visitor, payload_));
}

template<typename B>
B Field<B>::operator<(const Field &rhs) const {
    B geq = (rhs >= *this);
    return  !geq;
}

template<typename B>
B Field<B>::operator<=(const Field &rhs) const {
    return rhs >= *this;
}

template<typename B>
B Field<B>::operator>(const Field &rhs) const {
    return  rhs < *this;
}


template<typename B>
PlainField  Field<B>::reveal(const int &party) const {
    RevealVisitor visitor;
    visitor.party = party;
    visitor.type = type_;

    Value revealed = boost::apply_visitor(visitor, payload_);
    FieldType resType = TypeUtilities::toPlain(type_);

    size_t strLength = 0;
    if (type_ == FieldType::SECURE_STRING || type_ == FieldType::STRING) {
        strLength = string_length_;
    }
    return PlainField(resType, revealed, strLength);
}


template<typename B>
std::string Field<B>::toString() const {
    if(type_ == FieldType::INVALID)
        return "INVALID";
    ToStringVisitor visitor;
    visitor.type = type_;
    return boost::apply_visitor(visitor, payload_);

}

template<typename B>
void Field<B>::serialize(int8_t *dst) const {
    SerializeVisitor visitor;
    visitor.string_length_ = string_length_;
    visitor.dst_ = dst;
    boost::apply_visitor(visitor, payload_);
}



template<typename B>
SecureField Field<B>::secretShare(const PlainField  *field, const FieldType &type, const size_t &strLength, const int &myParty,
                           const int &dstParty) {


    SecretShareVisitor visitor;
    visitor.dstParty = dstParty;
    visitor.myParty = myParty;
    Value input = (myParty == dstParty) ? field->payload_ : FieldFactory<B>::getZero(type).payload_; // won't be used if receiving encrypted value from other party


    Value result = boost::apply_visitor(visitor, input);
    FieldType resType = TypeUtilities::toSecure(type);
    return SecureField(resType, result, strLength);

}


template<typename B>
void Field<B>::compareAndSwap(const B & choice, Field & lhs, Field & rhs) {
    Field<B> t  = If(choice, lhs, rhs);
    lhs = If(choice, rhs, lhs);
    rhs = t;
}



template<typename B>
Field<B> Field<B>::If(const B &choice, const Field &lhs, const Field &rhs) {
    assert(lhs.getType() == rhs.getType());
    assert(lhs.string_length_ == rhs.string_length_);

    SelectVisitor visitor;
    visitor.choiceBit = choice;
    visitor.rhs = rhs.payload_;
    return Field(lhs.getType(), boost::apply_visitor(visitor, lhs.payload_), lhs.string_length_);
}

template<typename B>
Field<B> Field<B>::operator+(const Field &rhs) const {
    assert(type_ == rhs.getType());

    PlusVisitor visitor;
    visitor.rhs = rhs.payload_;

    Value result = boost::apply_visitor(visitor, payload_);
    return Field(type_, boost::apply_visitor(visitor, payload_), string_length_);

}

template<typename B>
Field<B> Field<B>::operator-(const Field &rhs) const {
    assert(type_ == rhs.getType());

    MinusVisitor visitor;
    visitor.rhs = rhs.payload_;

    Value result = boost::apply_visitor(visitor, payload_);
    return Field(type_, boost::apply_visitor(visitor, payload_), string_length_);
}


template<typename B>
Field<B> Field<B>::operator*(const Field &rhs) const {
    assert(type_ == rhs.getType());

    MultiplyVisitor visitor;
    visitor.rhs = rhs.payload_;

    Value result = boost::apply_visitor(visitor, payload_);
    return Field(type_, boost::apply_visitor(visitor, payload_), string_length_);
}

template<typename B>
Field<B> Field<B>::operator/(const Field &rhs) const {
    assert(type_ == rhs.getType());

    DivisionVisitor visitor;
    visitor.rhs = rhs.payload_;

    Value result = boost::apply_visitor(visitor, payload_);
    return Field(type_, boost::apply_visitor(visitor, payload_), string_length_);

}

template<typename B>
Field<B> Field<B>::operator%(const Field &rhs) const {
    assert(type_ == rhs.getType());

    ModulusVisitor visitor;
    visitor.rhs = rhs.payload_;

    Value result = boost::apply_visitor(visitor, payload_);
    return Field(type_, boost::apply_visitor(visitor, payload_), string_length_);
}


template class vaultdb::Field<bool>;
template class vaultdb::Field<emp::Bit>;







