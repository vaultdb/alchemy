package org.vaultdb.db.data.field;

import java.io.Serializable;
import java.util.BitSet;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;

public class BooleanField extends Field implements Serializable {
public boolean value;
	
	public BooleanField(SecureRelDataTypeField attr, boolean v, SqlTypeName sqlType) {
		super(attr, sqlType);
		value = v;
	}
	
	public BooleanField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
		super(attr, sqlType);		
		value = false;
	}

	
	
	
	@Override
	public String serializeToBinaryString() {
		return (value) ? "1" : "0";
	}
	
	public boolean getValue() {
		return value;
	}
	
	@Override
	public String toString() {
		return (value) ? "true" : "false";
	}
	
	@Override
    public int hashCode() {
		int hash = Boolean.hashCode(value);
        
        return hash;
	}
	

	public boolean equals(Object o) {
		if(o instanceof IntField) {
			BooleanField bField = (BooleanField) o;
			if(bField.value == this.value) {
				return true;
			}
		}
		return false;
	}

	
	@Override
	public int childCompare(Field f) {
		if(f instanceof BooleanField) {
			Boolean lhs = value;
			Boolean rhs = ((BooleanField) f).getValue();
			return lhs.compareTo(rhs);
		}
		return 0;
	}

	
	@Override
	public void setValue(String source, int sourceOffset) {
		String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
		value = Boolean.parseBoolean(rawBits);
	}
	
	@Override
	public void deserialize(BitSet src) {
		assert(src.size() == this.size());
		value = src.get(0);
	}
	
	@Override
	public byte[] getBytes() {
		byte bytes[] = new byte[1];
		bytes[0] = (byte)(value ? 1 : 0);		
		return bytes;
	}


}
