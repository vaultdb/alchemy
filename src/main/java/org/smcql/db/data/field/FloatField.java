package org.smcql.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;

import java.io.Serializable;
import java.util.BitSet;

public class FloatField extends Field  implements Serializable {

    public Float value;
    int exponent;
    int fraction;

    public FloatField(SecureRelDataTypeField attr, Float v, SqlTypeName sqlType) {
        super(attr, sqlType);
        value = v;
        exponent = Float.floatToIntBits(v) & 0x7F800000;
        fraction = Float.floatToIntBits(v) & 0x807FFFFF;
    }

    public FloatField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
        super(attr, sqlType);
        value = 0.0f;
        exponent = 0;
        fraction = 0;
    }



    @Override
    public String serializeToBinaryString() {
        String binString = Integer.toBinaryString(Float.floatToIntBits(value));
        //System.out.println(value);
        //System.out.println(binString);
        while(binString.length() < 32){
            if(value < 0){
                binString = "1" + binString;
            }else{
                binString = "0" + binString;
            }
        }
        assert (binString.length() == 32);
        return binString;
    }

    public float getValue() {
        return value;
    }

    @Override
    public String toString() {
    	return Float.toString(value);
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
    public void deserialize(BitSet src) {
        assert(src.size() == this.size());
        int temp = 0;
        for(int i = 0; i < src.size(); ++i) {
        	boolean b = src.get(i);
            temp = (temp << 1) | (b ? 1 : 0);   
        }
        value = Float.intBitsToFloat(temp);
    }

}
