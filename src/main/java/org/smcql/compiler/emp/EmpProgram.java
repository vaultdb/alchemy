package org.smcql.compiler.emp;

import java.net.InetAddress;
import java.net.UnknownHostException;

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
    public  boolean[] runProgram() {
    	return null;
    }
    

    public String helloWorld() {
    	String className = this.getClass().getName();
    	return "I am a " + className + "!";
    }

}
