package org.vaultdb.executor.smc.io;

import java.util.BitSet;

import org.vaultdb.type.SecureRelRecordType;

public class SecureArray {
	 BitSet payload; // raw XOR-revealed tuples including dummies
	 BitSet dummyTags; // XOR-revealed dummy tags
	
	transient public SecureRelRecordType schema; // output schema
	int tupleCount = 0;
	int tupleSize = 0;
	int tuplesWritten = 0;
	
	
	public SecureArray(BitSet data, BitSet tags, SecureRelRecordType aSchema) {
		payload = data;
		dummyTags = tags;
		schema = aSchema;
		
		
		tupleSize = aSchema.size();
		
		assert(payload.size() % tupleSize == 0);
		tupleCount = payload.size() / tupleSize;
	}
	
	public SecureArray(int tupleCount, int tupleSize, SecureRelRecordType aSchema) {
	    this.tupleCount = tupleCount;
	    this.tupleSize = tupleSize;
	    
	    schema = aSchema;
	    dummyTags = new BitSet(tupleCount);
	    payload = new BitSet(tupleSize * tupleCount);
	}

	// payload and dummy tags are still encrypted
	public BitSet getPayload() { 
		return payload;
	}
	
	public BitSet getDummyTags() {
		return dummyTags;
	}
	

	public void setPayload(BitSet data) {
		payload = data;
		
	}

	public SecureRelRecordType getSchema() {
		return schema;
	}
	

	
	public void appendArray(SecureArray src) {
		BitSet srcPayload = src.getPayload();
		BitSet srcDummyTags = src.getDummyTags();
		int srcTupleCount = src.tupleCount;
		int writeIdx = tuplesWritten * tupleSize;
		
		for(int i = 0; i < srcTupleCount; ++i) { // for each tuple
			dummyTags.set(tuplesWritten, srcDummyTags.get(i)); // copy dummyTag
			
			for(int j = 0; j < tupleSize; ++j) { // copy out tuple's bits
				payload.set(writeIdx, srcPayload.get(j));
				++writeIdx;
			}
			
			++tuplesWritten;
		}
		
	}
	
}
