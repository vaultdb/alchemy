package org.vaultdb.compiler.emp;

import org.apache.calcite.util.Pair;

import com.google.protobuf.InvalidProtocolBufferException;

import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Map;

public class EmpProgram {

	protected int party = 0;
	protected int port = 0;
	protected String generatorHost = null; // hostname or IP address for Alice
	protected String outputString = null;
	protected BitSet outputBits = null;

	protected Map<String, String> inputs = new HashMap<String, String>();
	protected Map<String, Pair<Long, Long>> obliviousCardinalities = new HashMap<String, Pair<Long, Long>>();


	public EmpProgram(int aParty, int aPort) {
		party = aParty;
		port = aPort;
		
	}
	
	public void setGeneratorHost(String host) throws UnknownHostException {
		// if it is a hostname, resolve it to an IP address
		InetAddress address = InetAddress.getByName(host); 
		generatorHost = address.getHostAddress();

	}
	
	// delegate to implementing classes
    public void runProgram() throws Exception {

    }

	public DBQueryProtos.Table runProgramGetProto() throws InvalidProtocolBufferException {
		return null;
	}


	public String getOutputString() {
    	return outputString;
    	
    }
    
	protected QueryTable getInput(String sql) throws Exception {
    	SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(sql);
    	String workerId = System.getProperty("workerId");	
    	
					
		// Perform plaintext execution, which does not reduce cardinality, but add dummyTags
    	QueryTable table = SqlQueryExecutor.query(sql, outSchema, workerId);

    	
		return table;

    }

	
    protected String getObliviousInput(String name, String sql) throws Exception {
    	QueryTable baseTable = getInput(sql);
    	int tupleLength = baseTable.tupleSize() + 1; // for dummy tag
    	Pair<Long, Long> obliviousLength = obliviousCardinalities.get(name);
    	String workerId = System.getProperty("workerId");
    	Long obliviousCardinality = (workerId.equals(ConnectionManager.getInstance().getAlice())) ?
    			obliviousLength.left : obliviousLength.right;

    	String output = new String();
    	long padding = obliviousCardinality - baseTable.tupleCount();
    	
    	
    	char[] chars = new char[tupleLength];
    	Arrays.fill(chars, '0');
    	chars[tupleLength - 1] = '1'; // dummyTag
    	String dummyTag = new String(chars);
    	
    	
    	// add dummy tag of false to all results
    	for(Tuple t : baseTable.tuples()) {
    		output += t.toBinaryString() + '0'; // 0 for dummy tag
    	}
    	
    	for(int i = 0; i < padding; ++i) {
    		output += dummyTag;
    	}
    	
    	return output;
    	
    	
    }
    

    public BitSet getOutputBits() {
    	return outputBits;
    	
    }
    
    // output size in bits
    public int getOutputLength() {
    	if(outputString == null)
    		return 0;
    	
    	return outputString.length();
    }

    public static BitSet stringToBitSet(String s) {
  	  BitSet b = new BitSet(s.length());


      for (int i = 0; i < s.length(); ++i) {
      	b.set(i,  (s.charAt(i) == '1') ? true : false);
      }

      return b;
    }

    
    public String helloWorld() {
    	String className = this.getClass().getName();
    	return "I am a " + className + "!";
    }
    
   
}
