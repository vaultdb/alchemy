package org.smcql.compiler.emp;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.BitSet;

import org.smcql.db.data.QueryTable;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

public class EmpProgram {

	protected int party = 0;
	protected int port = 0;
	protected String generatorHost = null; // hostname or IP address for Alice
	

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
    public  BitSet runProgram() throws Exception {
    	return null;
    }
    

    public String helloWorld() {
    	String className = this.getClass().getName();
    	return "I am a " + className + "!";
    }
    
   
}
