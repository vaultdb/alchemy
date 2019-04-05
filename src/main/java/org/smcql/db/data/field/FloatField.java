package org.smcql.db.data.field;

import org.smcql.type.SecureRelDataTypeField;

import java.io.Serializable;

public class FloatField extends Field  implements Serializable {

    public Float value;
    int exponent;
    int fraction;

    public FloatField(SecureRelDataTypeField attr, Float v) {
        super(attr);
        value = v;
        exponent = Float.floatToIntBits(v) & 0x7F800000;
        fraction = Float.floatToIntBits(v) & 0x807FFFFF;
    }

    public FloatField(SecureRelDataTypeField attr) {
        super(attr);
        value = 0.0f;
        exponent = 0;
        fraction = 0;
    }

    @Override
    public int size()  {
        return 32;
    }


    @Override
    public String serializeToBinaryString() {
        String binString = Integer.toBinaryString(Float.floatToIntBits(value));
        assert (binString.length() == 32);
        return binString;
    }

    public float getValue() {
        return value;
    }

    @Override
    public String toString() {
        return serializeToBinaryString();
    }

    @Override
    public int hashCode() {
        int hash = Float.hashCode(value);

        return hash;
    }


    public boolean equals(Object o) {
        if(o instanceof FloatField) {
            FloatField floatField = (FloatField) o;
            if(floatField.value.equals(this.value)) {
                return true;
            }
        }
        return false;
    }


    @Override
    public int childCompare(Field f) {
        if(f instanceof FloatField) {
            Float lhs = new Float(value);
            Float rhs = new Float(((FloatField) f).getValue());
            return lhs.compareTo(rhs);
        }
        return 0;
    }


    @Override
    public void setValue(String source, int sourceOffset) {
        String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
        value = Float.intBitsToFloat(Integer.parseInt(rawBits));
    }

    @Override
    public void deserialize(boolean[] src) {
        assert(src.length == this.size());
        int temp = 0;
        for (boolean b : src)
            temp = (temp << 1) | (b ? 1 : 0);
        value = Float.intBitsToFloat(temp);
    }

}
