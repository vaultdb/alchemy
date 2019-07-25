package org.smcql.executor.smc.io;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

import org.apache.calcite.sql.type.SqlTypeName;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.type.SecureRelRecordType;
import org.smcql.type.TypeMap;
import org.smcql.executor.smc.BasicSecureQueryTable;
import org.smcql.executor.smc.SlicedSecureQueryTable;
import org.apache.calcite.rel.type.RelDataType;


public class SecureOutputReader {

	
	
	public static QueryTable assembleOutput(BasicSecureQueryTable alice, BasicSecureQueryTable bob, SecureRelRecordType schema) throws Exception {
		
		if (schema == null)
			schema = alice.getSchema();
		
		
		int length = decodeInt(alice.getSecureNonNullLength(), bob.getSecureNonNullLength());
		
		//get schema from the honest broker (embedded in the smcqlqueryexecutor
		return decodeSignals(alice.getSecurePayload(), bob.getSecurePayload(), length * schema.size(), schema);	
	}
	
	public static QueryTable assembleOutput(SlicedSecureQueryTable alice, SlicedSecureQueryTable bob, SecureRelRecordType schema) throws Exception {
		
		if (schema == null)
			schema = alice.schema;
		
		QueryTable result = new QueryTable(schema);
		for (Tuple t : alice.slices.keySet()) {
			BasicSecureQueryTable a = alice.slices.get(t);
			BasicSecureQueryTable b = bob.slices.get(t);
			int length = decodeInt(a.getSecureNonNullLength(), b.getSecureNonNullLength());
			QueryTable sliceResult = decodeSignals(a.getSecurePayload(), b.getSecurePayload(), length * schema.size(), schema);
			result.addTuples(sliceResult);
		}
		
		return result;	
	}
	
		
	
	// Alice data: # of elements (dummy or real) |dummyTags|data array
	// Bob data: #of elements|data array
	public static QueryTable assembleOutput(String alice, String bob, SecureRelRecordType schema) throws Exception{
		boolean[] aData = readSignals(alice);
		boolean[] bData = readSignals(bob);

		GCSignal R = aData[0];
		int lengthBits = TypeMap.getInstance().sizeof(SqlTypeName.INTEGER);
		
		int bitsToCopy = aData.length - lengthBits - 1;

		boolean[] aLength = new boolean[lengthBits];
		boolean[] bLength = new boolean[lengthBits];
		boolean[] aBits = new boolean[bitsToCopy];
		boolean[] bBits = new boolean[bitsToCopy];
		
		System.arraycopy(aData, 1, aLength, 0, lengthBits);
		System.arraycopy(bData, 0, bLength, 0, lengthBits);

		System.arraycopy(aData, lengthBits + 1, aBits, 0, bitsToCopy);
		System.arraycopy(bData, lengthBits, bBits, 0, bitsToCopy);

		int length = decodeInt(aLength, bLength, R);
		
		return decodeSignals(aBits, bBits, length * schema.size(), R, schema);
	}

	

	public static int decodeInt(boolean[] aData, boolean[] bData) throws Exception {
		int intSize = TypeMap.getInstance().sizeof(SqlTypeName.INTEGER);
		boolean[] bits = new boolean[intSize];
		int value = 0;
		assert(aData.length == bData.length && aData.length == intSize);
		
		
		for(int i = 0; i < intSize; ++i) {
			bits[i] = aData[i] ^ bData[i];
		}
		
		bits = Tuple.reverseBits(bits);
		for (boolean b : bits)
			value = (value << 1) | (b ? 1 : 0);
		
		return value;
	}
	
	
	
	public static long decodeLong(GCSignal[] aData, GCSignal[] bData, GCSignal R) throws Exception {
		boolean[] bits = new boolean[64];
		long value = 0L;
		assert(aData.length == bData.length && aData.length == 64);
		
		for(int i = 0; i < aData.length; ++i) {
			GCSignal aBit = aData[i];
			GCSignal bBit = bData[i];
			if(aBit.equals(bBit)) {
				bits[i] = false;
			}
			else if((R.xor(aBit)).equals(bBit)) {
				bits[i] = true;
			}
			else {
				throw new Exception("Bad label in output!");
			}
		}

		bits = Tuple.reverseBits(bits);
		for (boolean b : bits)
			value = (value << 1) | (b ? 1 : 0);
		
		return value;
	}
	
	
	// elements = # of bits to read
	public static QueryTable decodeSignals(boolean[] aData, boolean[] bData, int elements,  SecureRelRecordType schema) throws Exception {		
		assert(aData.length >= elements);
		assert(bData.length >= elements);
		
		
		boolean[] plaintext = new boolean[elements];
		
		for(int i = 0; i < elements; ++i) {
			plaintext[i] = aData[i] ^ bData[i];
		}		
		return new QueryTable(plaintext, schema);

	}
}
