package org.smcql.compiler.emp;

import java.lang.reflect.Method;

public class EmpProgram  {


	protected EmpParty party;
	Object subclass;
	

	// JNI seems to only cooperate in a remote setting with primitives as inputs
	// EmpProgram(EmpParty) fails
	public EmpProgram(int aParty, int aPort) {
		party = new EmpParty(aParty, aPort);
	}
	
	public void configure(EmpParty aParty) {
		party = aParty;
	
	}
	
	public void setSubclass(Object aSubclass) {
		subclass = aSubclass;
	}
	
	// delegate to children
    public boolean[] runProgram() {
    
    	boolean[] results = null;
    	if(subclass != null) {
    		try {
    			Method runMethod = subclass.getClass().getMethod("runProgram");
    			results = (boolean[]) runMethod.invoke(subclass);
    			
    		} catch (Exception e) {
    			
    		}
    	}
    	
    	return results;
    }
    
    
    public String helloWorld() {
    	return "I am an EmpProgram!";
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
