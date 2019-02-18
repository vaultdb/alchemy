package org.smcql.codegen.smc.compiler.emp;

public class EmpProgram  {

	protected int party = 1;
	protected int port = 54321;
	
	public EmpProgram(int aParty, int aPort) {
		configure(aParty, aPort);
	}
	
	public void configure(int aParty, int aPort) {
		party = aParty;
		port = aPort;
	
	}
	
	// delegate to children
    public  boolean[] runProgram() {
    	return null;
    }
    
	// Source: http://www.java2s.com/Code/Android/Date-Type/ConvertabytearraytoabooleanarrayBit0isrepresentedwithfalseBit1isrepresentedwith1.htm
	public static boolean[] byteArray2BitArray(byte[] bytes) {
	    boolean[] bits = new boolean[bytes.length * 8];
	    for (int i = 0; i < bytes.length * 8; i++) {
	      if ((bytes[i / 8] & (1 << (7 - (i % 8)))) > 0)
	        bits[i] = true;
	    }
	    return bits;
	  }
}
