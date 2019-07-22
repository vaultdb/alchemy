package org.smcql.compiler.emp;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.BitSet;

public class EmpProgram {

	protected int party = 0;
	protected int port = 0;
	protected String generatorHost = null; // hostname or IP address for Alice
	protected String outputString = null;
	protected BitSet outputBits = null;

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
    public  void runProgram() throws Exception {

    }
    

    public String getOutputString() {
    	return outputString;
    	
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
