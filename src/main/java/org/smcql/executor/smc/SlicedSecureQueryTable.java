package org.smcql.executor.smc;

import java.io.Serializable;
import java.util.BitSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.smcql.config.SystemConfiguration.Party;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.type.SecureRelRecordType;
import org.smcql.executor.smc.io.SecureArray;
import org.smcql.executor.smc.runnable.SMCRunnable;
import org.smcql.plan.slice.SliceKeyDefinition;


import org.apache.commons.lang3.tuple.ImmutablePair;
import org.apache.commons.lang3.tuple.Pair;



public class SlicedSecureQueryTable implements SecureQueryTable, Serializable {

	public Map<Tuple, BasicSecureQueryTable> slices;
	public String bufferPoolKey;
	SliceKeyDefinition sliceKey;
	transient public SecureRelRecordType schema;
	transient SMCRunnable parent;
	
	// map of plaintext value to the secret shared outputs
	transient Iterator<Entry<Tuple, BasicSecureQueryTable>> sliceItr;
	
	QueryTable plaintextOutput;
	protected BasicSecureQueryTable aPlaintext, bPlaintext; // this will be skipped by sliceItr, but included in declassify and getSecureArray
	boolean merged = false;
	boolean written = false; // for after output is initialized 
	SecureArray array;
	
	
    public static String getKey(OperatorExecution op) {
        Party p = op.getParty();
        String suffix = (p == Party.ALICE) ? "-gen" : "-eva";
        return op.packageName + "." + op.getWorkerId() + suffix;
        
    }
    
	public SlicedSecureQueryTable(OperatorExecution op, SMCRunnable p, boolean isRemote) throws Exception {
		parent = p;
		schema = op.outSchema;
		slices = new LinkedHashMap<Tuple, BasicSecureQueryTable>();
	
		
		bufferPoolKey = getKey(op);

		if(isRemote) {
			initializeRemotePlaintext();
		}
		else {
			initializeLocalPlaintext();
		}
		
		sliceItr = slices.entrySet().iterator(); 

		
		
	}
	
	public SlicedSecureQueryTable(SecureRelRecordType opSchema, SMCRunnable p, boolean isRemote) throws Exception {
		parent = p;
		schema = opSchema;
		slices = new LinkedHashMap<Tuple, BasicSecureQueryTable>();
		OperatorExecution op = parent.getRootOperator();
		
		bufferPoolKey = getKey(op);
		
		sliceItr = slices.entrySet().iterator(); 
	}
	
	public SlicedSecureQueryTable(OperatorExecution op, SMCRunnable p, BasicSecureQueryTable slice, Tuple t) throws Exception {
		parent = p;
		schema = op.outSchema;
		slices = new LinkedHashMap<Tuple, BasicSecureQueryTable>();
		slices.put(t, slice);
		
		bufferPoolKey = getKey(op);
		sliceItr = slices.entrySet().iterator(); 
	}

	
	// for collecting segment output
		public SlicedSecureQueryTable(OperatorExecution op, SMCRunnable p) {
			slices = new LinkedHashMap<Tuple, BasicSecureQueryTable>();
			bufferPoolKey = getKey(op);
			schema = op.outSchema;
			parent = p;
			
		}

	private void initializeRemotePlaintext() {
		/* This happens in EMP land now
		 * Need to integrate it into the code generator
		 
		 int sliceCount = parent.getInt();
		 
		for(int i = 0; i < sliceCount; ++i) {

			Pair<Tuple, BasicSecureQueryTable> slice = SMCUtils.prepareRemoteSlicedPlaintext(env, parent);
			BasicSecureQueryTable table = slice.getRight();
			table.array.schema = array.schema;
			
			slices.put(slice.getLeft(), table);
			
		}
		*/
		
	}

	
	

	private void initializeLocalPlaintext() throws Exception {
		/** This happens in EMP land now, need to integrate it there 
		 * by passing in a 2D array of tuples from JNI wrapper
		 */
		/*
		ConnectionManager cm = ConnectionManager.getInstance();
		OperatorExecution operator = parent.getRootOperator();
        Connection c = cm.getConnection(operator.getWorkerId());

        
        QueryTable plainInput =  SqlQueryExecutor.query(operator.outSchema, operator.getSourceSQL(), c);	

		Map<Tuple, List<Tuple> > plainSlices = plainInput.asSlices(operator.parentSegment.sliceSpec);
		
		int sliceCount = plainSlices.keySet().size();	
		parent.sendInt(sliceCount);
		
		for(Tuple key : plainSlices.keySet()) {
			BasicSecureQueryTable table = SMCUtils.prepareLocalPlaintext(key, plainSlices.get(key), env, parent);
			table.array.schema = array.schema;
			slices.put(key, table);
		}
		
		bufferPoolKey = getKey(operator);
		sliceItr = slices.entrySet().iterator(); 
*/
		
	}
	

	
	
	@Override
	public String getBufferPoolKey() {
		return bufferPoolKey;
	}


	
	public void resetSliceIterator() {
		sliceItr = slices.entrySet().iterator(); 
	}
	
	public boolean hasNext() {
		return sliceItr.hasNext();
	}
	
	public Pair<Tuple, SecureArray> getNextSlice() throws Exception {
		if(!sliceItr.hasNext())
			return null;
		
		Map.Entry<Tuple, BasicSecureQueryTable> entry = sliceItr.next();
		BasicSecureQueryTable table = entry.getValue();
		SecureArray secArray = table.getSecureArray();
		Tuple key = entry.getKey();
		
		return new ImmutablePair<Tuple, SecureArray>(key, secArray);
	}
	
	@Override
	public QueryTable declassify(SecureQueryTable table) throws Exception {
		if(!(table instanceof SlicedSecureQueryTable)){
			throw new Exception("Cannot decode unmatched tables!");
		}

		SlicedSecureQueryTable otherTable = (SlicedSecureQueryTable) table;
		
		
		QueryTable result = new QueryTable(schema);

		for(Tuple key : slices.keySet()) {
			BasicSecureQueryTable a = this.slices.get(key);
			BasicSecureQueryTable b = otherTable.slices.get(key);
			QueryTable keyTable = a.declassify(b);
			result.addTuples(keyTable);
		}

		
		result.addTuples(this.plaintextOutput);
		result.addTuples(otherTable.plaintextOutput);
		
		
		
		return result;
	}

	
	// flatten it into a single SecureArray
	// stitch it into one array for input to a secure operator
	// is the plaintext output replicated?  No, because it is a parallel secure step
	// is it input of bob, alice or both? Need an additional merge step to cover the two

	@Override
	public SecureArray getSecureArray() throws Exception {
		int tupleCount = 0;
		int tupleSize = schema.size();
		for(BasicSecureQueryTable t : slices.values()) {
			tupleCount += t.getSecurePayload().size() /  tupleSize;
		}
		
		SecureArray output = new SecureArray(tupleCount, tupleSize, schema);
		
		
		for(Tuple t : slices.keySet()) {
			
			
			SecureArray value = slices.get(t).getSecureArray();
			output.appendArray(value);
		}	
		
		return output;
		
	}
	
	
	
	@Override
	public void setPlaintextOutput(QueryTable pc) throws Exception {
		
		plaintextOutput = pc;		
	}



	
	
	public SecureArray getSlice(Tuple value) throws Exception {
		BasicSecureQueryTable basic = slices.get(value);
		if(basic != null)
			return basic.getSecureArray();
		
		return null;
	}


	public void addSlice(Tuple key, BitSet payload, BitSet dummyTags) {
		BasicSecureQueryTable table = new BasicSecureQueryTable(payload, dummyTags, schema, parent);
		slices.put(key, table);
		written = true;
		
	}





	@Override
	public QueryTable getPlaintextOutput() {
		return plaintextOutput;
	}

	



	@Override
	public BitSet getSecurePayload() throws Exception {
		SecureArray array = this.getSecureArray();
		return array.getPayload();
	}

	@Override
	public BitSet getDummyTags() throws Exception {
		SecureArray array = this.getSecureArray();
		return array.getDummyTags();
	}

	@Override
	public QueryTable declassify(SecureQueryTable bob, SecureRelRecordType schema) throws Exception {
		if(this.schema == null) {
			this.schema = schema;
		}
		
		return declassify(bob);
	}





}
