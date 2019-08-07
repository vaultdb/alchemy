package org.smcql.db.data.field;

import java.io.Serializable;
import java.util.BitSet;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.TypeMap;

public class IntField extends Field  implements Serializable   {

	public long value;
	
	public IntField(SecureRelDataTypeField attr, int v, SqlTypeName sqlType) {
		super(attr, sqlType);
		value = v;
		
	}
	
	public IntField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
		super(attr, sqlType);		
		value = 0;
	}

	
	
	@Override
	public String serializeToBinaryString() {
		String binString = Long.toBinaryString(value);
		
		// pad it for length
		while(binString.length() < this.size()) {
			binString = '0' + binString;
		}

	
		return binString;
	}
	
	public long getValue() {
		return value;
	}
	
	@Override
	public String toString() {
		return Long.toString(value);
	}
	
	@Override
    public int hashCode() {
		int hash = Long.hashCode(value);
        
        return hash;
	}
	

	public boolean equals(Object o) {
		if(o instanceof IntField) {
			IntField intField = (IntField) o;
			if(intField.value == this.value) {
				return true;
			}
		}
		return false;
	}

	
	@Override
	public int childCompare(Field f) {
		if(f instanceof IntField) {
			Long lhs = new Long(value);
			Long rhs = new Long(((IntField) f).getValue());
			return lhs.compareTo(rhs);
		}
		return 0;
	}

	
	@Override
	public void setValue(String source, int sourceOffset) {
		String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
		value = Integer.parseInt(rawBits, 2);
		
	}
	
	@Override
	public void deserialize(BitSet src) {
		value = 0;
		boolean b;

		for(int i = 0; i < this.size(); ++i) {
			 b = src.get(i);
			 value = (value << 1) | (b ? 1 : 0);
		}
		
	}
	
}
