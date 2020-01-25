package org.vaultdb.db.data.field;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.sql.Timestamp;
import java.util.BitSet;

import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelDataTypeField;



public class TimestampField extends Field implements Serializable  {

	/**
	 * 
	 */
	private static final long serialVersionUID = -6750183365124209223L;
	public Timestamp time;
	
	
	TimestampField(SecureRelDataTypeField attr, Timestamp timestamp, SqlTypeName sqlType) {
		super(attr, sqlType);
		time = timestamp;
		timestamp.getTime();

	}
	
	public TimestampField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
		super(attr, sqlType);
	}

	
	@Override
	public String serializeToBinaryString() {
		long epoch = time.getTime();
		String binString = Long.toBinaryString(epoch);
		
		while(binString.length() < this.size()) {
			binString = '0' + binString;
		}
		
		return binString;

	}
	
	
	public String toString() {
		return new String(Long.toString(time.getTime()));
	}

	
	@Override
    public int hashCode() {
        Long hash = time.getTime();
        return hash.hashCode();

	}
	
	public boolean equals(Object o) {
		if(o instanceof IntField) {
			TimestampField field = (TimestampField) o;
			if(field.time.getTime() == this.time.getTime() ) {
				return true;
			}
		}
		return false;
	}

	@Override
	public int childCompare(Field f) {
		if(f instanceof TimestampField) {
			Long lhs = new Long(time.getTime());
			Long rhs = new Long(((TimestampField) f).time.getTime());
			return lhs.compareTo(rhs);
		}
		return 0;
	}

	
	@Override
	public void setValue(String source, int sourceOffset) {
		String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
		Long epoch = Long.parseLong(rawBits, 2);
		time = new Timestamp(epoch);
		
	}
	
	
	@Override
	public void deserialize(BitSet src) {
		
		assert(src.size() == this.size());
		long epoch = 0;
		boolean b;
		
		for(int i = 0; i < src.size(); ++i) {
			b = src.get(i);
			epoch = (epoch << 1) | (b ? 1 : 0);

		}

		time = new Timestamp(epoch);
		
	}

	@Override 
	public byte [] getBytes() {	
		ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
	    buffer.putLong(time.getTime()); // serialize to epoch
	    return buffer.array();
	}
	
	
}
