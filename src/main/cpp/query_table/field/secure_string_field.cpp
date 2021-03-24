#include "secure_string_field.h"
#include "util/utilities.h"
#include "field_factory.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureStringField &aValue) {
    return os << aValue.toString();
}


// use default constructor to avoid dealing with string length
SecureStringField::SecureStringField(const Field &srcField) : Field(srcField) { }

SecureStringField::SecureStringField(const SecureStringField &src) : Field(src) {
}



SecureStringField::SecureStringField(const int8_t *src, const size_t & strLength) : Field(Field::deserialize(FieldType::SECURE_STRING, strLength, src)) { }

// src is a StringField
SecureStringField::SecureStringField(const vaultdb::StringField *src, const size_t &strLength, const int &myParty,
                                     const int &dstParty) : Field(FieldType::SECURE_STRING, strLength){
    std::string input = (myParty == dstParty) ?
                        src->getStringValue() :
                        std::to_string(0);


    assert(input.length() <= strLength);   // while loop will be infinite if the string is already oversized

    while(input.length() != strLength) { // pad it to the right length, -1 to remove null terminator
        input += " ";
    }

    std::string inputReversed = input;
    std::reverse(inputReversed.begin(), inputReversed.end());
    bool *bools = Utilities::bytesToBool((int8_t *) inputReversed.c_str(), strLength);

    size_t stringBitCount = strLength * 8;

    emp::Integer payload = emp::Integer(stringBitCount, 0L, dstParty);
    if(myParty == dstParty) {
        emp::ProtocolExecution::prot_exec->feed((emp::block *)payload.bits.data(), dstParty, bools, stringBitCount);
    }
    else {
        emp::ProtocolExecution::prot_exec->feed((emp::block *)payload.bits.data(), dstParty, nullptr, stringBitCount);
    }


    delete [] bools;
    setValue(payload);
}



SecureStringField::SecureStringField(const emp::Integer &src) : Field(FieldType::SECURE_STRING, src.size() / 8) {
    setValue(src);
}


SecureStringField &SecureStringField::operator=(const SecureStringField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

SecureBoolField SecureStringField::operator>=(const SecureStringField &cmp) const {
    emp::Bit res =  (getPayload() >= cmp.getPayload());
    return SecureBoolField(res);
}

SecureBoolField SecureStringField::operator==(const SecureStringField &cmp) const {
    emp::Bit res = ((getPayload()) == (cmp.getPayload()));
    return  SecureBoolField(res);
}

SecureStringField SecureStringField::selectValue(const SecureBoolField &choice, const SecureStringField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Integer res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureStringField(res);
}

SecureStringField SecureStringField::operator&(const SecureStringField &right) const {
    return SecureStringField((getPayload()) & right.getPayload());
}

SecureStringField SecureStringField::operator^(const SecureStringField &right) const {
    return SecureStringField((getPayload()) ^ (right.getPayload()));
}

SecureStringField SecureStringField::operator|(const SecureStringField &right) const {
    return SecureStringField((getPayload()) | (right.getPayload()));
}

void SecureStringField::ser(int8_t *target) const {
    memcpy(target, (int8_t *) getPayload().bits.data(), allocated_size_);
}

