package org.vaultdb.db.data.field;

import java.io.IOException;
import java.io.Serializable;
import java.util.BitSet;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;

public class CharField extends Field implements Serializable   {
	/**
	 * 
	 */
	private static final long serialVersionUID = 7638908984744040905L;
	public String value;
	
	
	CharField(SecureRelDataTypeField attr, SqlTypeName sqlType) throws IOException {
		super(attr, sqlType);
		value = new String();
		size = attr.size();
	}

	public CharField(SecureRelDataTypeField attr, String value, SqlTypeName sqlType) throws Exception {
		super(attr, sqlType);
		this.value = value;
		if(value != null && value.length() > (attr.size() / 8)) {
			throw new Exception("String exceeds specified size!" + value + " at " + value.length() + " for attribute " + attr);
		}
		size = attr.size();

	}
	


	@Override
	public String serializeToBinaryString() {
		String ret = new String();
		
		if(value != null) {
			for(char c : value.toCharArray()) {
				ret += serializeChar(c);
			}
		}
		
		
		
		while(ret.length() < size()) {
			ret += '0';
		}
		

		return ret;
	}
	
	private String serializeChar(char c) {

		String ret = Integer.toBinaryString((int) c);
		while(ret.length() < 8) {
			ret = "0" + ret;
		}
		
		return ret;
	}
	
	public String toString() {
		return new String(value);
	}
	
	@Override
    public int hashCode() {
        return value.hashCode();
        
	}
	
	public boolean equals(Object o) {
		if(o instanceof CharField) {
			CharField charField = (CharField) o;
		
			while(charField.value.length() < value.length()) { // fix null padding
				charField.value += "\0";
			}
			
			if(charField.value.equals(this.value)) {
				return true;
			}
		}
		return false;
	}

	@Override
	public int childCompare(Field f) {
		if(f instanceof CharField) {
			return value.compareTo(((CharField) f).value);
		}
		return 0;
	}

	
	@Override
	public void setValue(String source, int startOffset) {
		value = new String();
		 
		for(int i = 0; i < size/8; ++i) {
			value += getCharacter(source, startOffset + i * 8);
		}
		
		
	}
	
	private char getCharacter(String source, int startOffset) {
		String rawBits = source.substring(startOffset, startOffset + 8);
		while(rawBits.length() < this.size()) {
			rawBits += "0";
		}
		

		int value = Integer.parseInt(rawBits, 2);
		
		return (char) value;
		
	}
	
	@Override
	public void deserialize(BitSet src) {
		//assert(src.size() == this.size); 
		
		value = new String();
		int chars = this.size / 8;
		
		for(int i = 0; i < chars; ++i)
		{
			BitSet bits =  src.get(i*8, (i+1)*8);
			value += deserializeChar(bits);
			
		}
		
	}

	
	
	private char deserializeChar(BitSet bits) {

	    int n = 0;
	    for(int i = 0; i < 8; ++i) {
	    	boolean b = bits.get(i);
	        n = (n << 1) | (b ? 1 : 0);
	    }
	    
	    System.out.println("Decoded character " + (char) n + " from " + n + " and " + bits);
	    
	    return (char) n;
	}

}
