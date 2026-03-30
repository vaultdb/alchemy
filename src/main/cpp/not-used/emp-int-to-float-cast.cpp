// https://stackoverflow.com/questions/20302904/converting-int-to-float-or-float-to-int-using-bitwise-operations-software-float
// range: [-2^24, 2^24]
Float castIntToFloat(const Integer &input) {

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
    Integer shiftOffset = If(leftShift, firstOneIdx - twentyThree, twentyThree - firstOneIdx);
    Integer shifted = If(leftShift, unsignedInput >> shiftOffset, unsignedInput << shiftOffset);

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
    output = If(input == zero, Float(0.0, PUBLIC), output);
    output = If(input < minInt, Float(INT_MIN, PUBLIC), output);
    output = If(input > maxInt, Float(INT_MAX, PUBLIC), output);

    return output;

}
