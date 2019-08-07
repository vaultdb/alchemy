package org.vaultdb.executor.smc;

import java.io.Serializable;
import java.util.BitSet;

import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.smc.io.SecureArray;
import org.vaultdb.type.SecureRelRecordType;


public interface SecureQueryTable extends Serializable {
	

	public String getBufferPoolKey();
	
	public QueryTable declassify(SecureQueryTable bob) throws Exception;
	public QueryTable declassify(SecureQueryTable bob, SecureRelRecordType schema) throws Exception;
	public SecureArray getSecureArray() throws Exception;
	
	// for mixed computation
	public void setPlaintextOutput(QueryTable pc) throws Exception;

	
	public BitSet getSecurePayload() throws Exception;
	
	public BitSet getDummyTags() throws Exception;

	public QueryTable getPlaintextOutput();
	
}
