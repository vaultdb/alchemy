#include "secure_float_field.h"

using namespace vaultdb;
using namespace emp;

std::ostream &operator<<(std::ostream &os, const SecureFloatField &aValue) {
    return os << aValue.toString();
}


SecureFloatField::SecureFloatField(const Field &srcField) : Field(FieldType::SECURE_FLOAT) {
    setValue(srcField.getValue<emp::Float>());

}

SecureFloatField::SecureFloatField(const SecureFloatField &src) : Field(src) { }



SecureFloatField::SecureFloatField(const int8_t *src) : Field(Field::deserialize(FieldType::SECURE_INT, 0, src)){ }

SecureFloatField::SecureFloatField(const FloatField *src, const int &myParty, const int &dstParty) : Field(FieldType::SECURE_FLOAT){
    float_t toEncrypt = (myParty == dstParty) ? src->getValue<float_t>() : 0;
    emp::Float payload = emp::Float(toEncrypt, dstParty);
    setValue(payload);
}



SecureFloatField::SecureFloatField(const emp::Float &src) : Field(FieldType::SECURE_FLOAT) {
    setValue(src);
}


SecureFloatField &SecureFloatField::operator=(const SecureFloatField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

SecureBoolField SecureFloatField::operator>=(const SecureFloatField &cmp) const {
    emp::Float lhs = getPayload();
    emp::Float rhs = cmp.getPayload();
    emp::Bit res =  !(lhs.less_than(rhs));
    return SecureBoolField(res);
}

SecureBoolField SecureFloatField::operator==(const SecureFloatField &cmp) const {
    emp::Bit res = ((getPayload()).equal(cmp.getPayload()));
    return  SecureBoolField(res);
}

SecureFloatField SecureFloatField::selectValue(const SecureBoolField &choice, const SecureFloatField &other) const {
    emp::Bit selection =  choice.getPayload();
    emp::Float res =  emp::If(selection, getPayload(), other.getPayload());
    return SecureFloatField(res);
}

SecureFloatField SecureFloatField::operator&(const SecureFloatField &right) const {
    return SecureFloatField((getPayload()) & right.getPayload());
}

SecureFloatField SecureFloatField::operator^(const SecureFloatField &right) const {
    return SecureFloatField((getPayload()) ^ (right.getPayload()));
}

SecureFloatField SecureFloatField::operator|(const SecureFloatField &right) const {
    throw;
}



vaultdb::SecureFloatField vaultdb::SecureFloatField::operator+(const vaultdb::SecureFloatField &rhs) const {
    return SecureFloatField(getPayload() + rhs.getPayload());
}

void SecureFloatField::ser(int8_t *target) const {
    memcpy(target, (int8_t *) getPayload().value.data(), allocated_size_);
}

SecureFloatField SecureFloatField::toFloat(const emp::Integer &input) {

        const Integer zero(32, 0, PUBLIC);
        const Integer one(32, 1, PUBLIC);
        const Integer maxInt(32, 1 << 24, PUBLIC); // 2^24
        const Integer minInt = Integer(32, -1 * (1 << 24), PUBLIC); // -2^24
        const Integer twentyThree(32, 23, PUBLIC);

        Float output(0.0, PUBLIC);

        Bit signBit = input.bits[31];
        Integer unsignedInput = input.abs();

        Integer firstOneIdx = Integer(32, 31, PUBLIC) - unsignedInput.leading_zeros().resize(32);

        Bit leftShift = firstOneIdx >= twentyThree;
        Integer shiftOffset = emp::If(leftShift, firstOneIdx - twentyThree, twentyThree - firstOneIdx);
        Integer shifted = emp::If(leftShift, unsignedInput >> shiftOffset, unsignedInput << shiftOffset);

        // exponent is biased by 127
        Integer exponent = firstOneIdx + Integer(32, 127, PUBLIC);
        // move exp to the right place in final output
        exponent = exponent << 23;

        Integer coefficient = shifted;
        // clear leading 1 (bit #23) (it will implicitly be there but not stored)
        coefficient.bits[23] = Bit(false, PUBLIC);


        // bitwise OR the sign bit | exp | coeff
        Integer outputInt(32, 0, PUBLIC);
        outputInt.bits[31] = signBit; // bit 31 is sign bit

        outputInt =  coefficient | exponent | outputInt;
        memcpy(&(output.value[0]), &(outputInt.bits[0]), 32 * sizeof(Bit));

        // cover the corner cases
        output = emp::If(input == zero, Float(0.0, PUBLIC), output);
        output = emp::If(input < minInt, Float(INT_MIN, PUBLIC), output);
        output = emp::If(input > maxInt, Float(INT_MAX, PUBLIC), output);

        return SecureFloatField(output);


}

SecureFloatField SecureFloatField::toFloat(const SecureIntField &f) {
    return toFloat(f.getPayload());
}


SecureFloatField SecureFloatField::toFloat(const SecureLongField &f) {
    return toFloat(f.getPayload());
}
