package org.smcql.codegen.smc;

public class EmpProgram {
	
    protected int party = 1;
    protected int port = 54321;
    
    public EmpProgram(int aParty, int aPort)  {
    	party = aParty;
    	port = aPort;
    	
    }
    
    // delegate to children
    public boolean[] runProgram() {
    	return null;
    }
}
