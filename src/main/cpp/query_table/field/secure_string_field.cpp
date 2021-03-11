#include "secure_string_field.h"
#include "util/utilities.h"

using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const SecureStringField &aValue) {
    return os << aValue.toString();
}


// use default constructor to avoid dealing with string length
SecureStringField::SecureStringField(const Field &srcField) : Field(srcField) { }

SecureStringField::SecureStringField(const SecureStringField &src) : Field(src) {
}



SecureStringField::SecureStringField(const int8_t *src, const size_t & strLength) : Field(Field::deserialize(FieldType::SECURE_STRING, strLength, src)) { }

SecureStringField::SecureStringField(const Field *src, const size_t &strLength, const int &myParty,
                                     const int &dstParty) : Field(FieldType::SECURE_STRING, strLength){
    std::string input = (myParty == dstParty) ?
                        src->getStringValue() :
                        std::to_string(0);

    size_t stringByteCount = input.size();

    assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

    while(input.length() != stringByteCount) { // pad it to the right length
        input += " ";
    }

    std::string inputReversed = input;
    std::reverse(inputReversed.begin(), inputReversed.end());
    bool *bools = Utilities::bytesToBool((int8_t *) inputReversed.c_str(), stringByteCount);

    size_t stringBitCount = stringByteCount * 8;

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

SecureStringField SecureStringField::select(const SecureBoolField &choice, const SecureStringField &other) const {
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

