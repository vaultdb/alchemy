package org.smcql.compiler.emp;

public class EmpProgram {

	protected int party = 0;
	protected int port = 0;
	protected String generatorHost = null; // hostname or IP address for Alice
	

	public EmpProgram(int aParty, int aPort) {
		party = aParty;
		port = aPort;
		
	}
	
	public void setGeneratorHost(String host) {
		generatorHost = host;
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
