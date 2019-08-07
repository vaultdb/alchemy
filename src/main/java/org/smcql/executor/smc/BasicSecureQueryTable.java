package org.smcql.executor.smc;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.BitSet;

import org.smcql.db.data.QueryTable;
import org.smcql.executor.smc.io.SecureArray;
import org.smcql.executor.smc.runnable.SMCRunnable;
import org.vaultdb.type.SecureRelRecordType;

public class BasicSecureQueryTable implements SecureQueryTable, Serializable {

	
	/**
	 * 
	 */
	private static final long serialVersionUID = -6328855454363807543L;
	public String bufferPoolKey;
	transient SMCRunnable parent = null;
	QueryTable plaintextOutput;
	int tupleSize;
	SecureArray array;
	
	// PI is null if this is part of a SlicedSecureQueryTable
	BasicSecureQueryTable plaintextInput;
	
	public BasicSecureQueryTable(BitSet data, BitSet dummyTags, SecureRelRecordType table, SMCRunnable r) {
		array = new SecureArray(data, dummyTags, table);

		parent = r;
		tupleSize = (table == null) ? 0 : table.size();
		
	}

	public BasicSecureQueryTable(BitSet data, int tSize, SMCRunnable r) {
		array.setPayload(data); // for use in SlicedSecureQueryTable
		parent = r;
		tupleSize = tSize;

	}
	
	

	// from EMP we receive dummy tags followed by tuple payload
	public BasicSecureQueryTable(BitSet data, SecureRelRecordType schema) {

		tupleSize = schema.size();
		int tupleCount = data.size() / (tupleSize + 1); // + 1 for dummy tags
		
		assert(tupleCount % (tupleSize + 1) == 0);
		
		BitSet dummyTags = data.get(0, tupleCount);
		BitSet tuples = data.get(tupleCount, data.size());
		
		array = new SecureArray(tuples, dummyTags, schema);
		
	}

	@Override
	public String getBufferPoolKey() {
		return bufferPoolKey;
	}


	// TODO: clean this up
	@Override
	public QueryTable declassify(SecureQueryTable other, SecureRelRecordType schema) throws Exception {
		/*
		if(!(other instanceof BasicSecureQueryTable)){
			throw new Exception("Cannot decode unmatched tables!");
		}
		
		
		if(schema == null) {
			schema = array.getSchema(); // use the one we have
		}
		
		BitSet plaintext = (BitSet) array.getPayload().clone();
		plaintext.xor(other.getSecurePayload()); // decrypted 
		
		BitSet dummyTags = (BitSet) array.getDummyTags().clone();
		dummyTags.xor(other.getDummyTags()); // decrypted


		return new QueryTable(plaintext, schema, true);
		*/
		return null;
	}

	// deprecated, this is for doing OT on plaintext inputs
	/*@Override
	public SecureArray getSecureArray() throws Exception {
		if(plaintextOutput == null) { // for use within a sliced operator
			int tupleSize = array.schema.size();
			int tupleCount = array.payload.length / tupleSize;
			SecureArray<GCSignal> input = Util.intToSecArray(localEnv, payload, tupleSize, tupleCount);
			input.setNonNullEntries(nonNullLength);
			
			return input;
		}
		
		// for use *after* a sliced operator
		return merger.merge(this, localEnv, runnable);
	}*/


	@Override
	public void setPlaintextOutput(QueryTable pc) throws Exception {
		plaintextOutput = pc;
		
	}

	private void writeObject(ObjectOutputStream out) throws IOException {
		out.writeObject(array);
		out.writeObject(bufferPoolKey);
		out.writeObject(plaintextOutput);
		out.writeInt(tupleSize);
		out.writeObject(plaintextInput);
		//out.writeObject(parent);		
	}
	
	
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		
		array = (SecureArray) ois.readObject();
				
		bufferPoolKey = (String)ois.readObject();
		plaintextOutput = (QueryTable) ois.readObject();
		tupleSize = ois.readInt();
		plaintextInput = (BasicSecureQueryTable) ois.readObject();
		
		//schema = (SecureRelRecordType)ois.readObject();
		//parent = (SMCRunnable)ois.readObject();
	}



	@Override
	public BitSet getSecurePayload() {
		return array.getPayload();
	}

	@Override
	public  BitSet getDummyTags() {
		return array.getDummyTags();
	}

	
	@Override
	public QueryTable getPlaintextOutput() {
		return plaintextOutput;
	}

	@Override
	public QueryTable declassify(SecureQueryTable bob) throws Exception {
		return declassify(bob, null);
	}

	@Override
	public SecureArray getSecureArray() throws Exception {
		return array;
	}

	public SecureRelRecordType getSchema() {
		
		return array.schema;
	}

	
}
