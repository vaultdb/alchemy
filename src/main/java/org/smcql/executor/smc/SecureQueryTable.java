package org.smcql.executor.smc;

import java.io.Serializable;
import java.util.BitSet;

import org.smcql.config.SystemConfiguration.Party;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.smc.io.SecureArray;
import org.smcql.type.SecureRelRecordType;


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
