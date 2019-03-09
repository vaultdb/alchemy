package org.smcql.compiler.emp;

public class EmpProgram {

	protected int party = 0;
	protected int port = 0;
	

	public EmpProgram(int aParty, int aPort) {
		party = aParty;
		port = aPort;
		
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
