package org.vaultdb.compiler.emp;


// simple party definition
public class EmpParty {
	
	int party = 0; // agnostic, not alice or bob
	int port = 54321;
	
	public EmpParty(int party, int port) {
		this.party = party;		
		this.port = port;
	}
	
	public EmpParty(String partyStr, int port)  {

		if(partyStr.equalsIgnoreCase("Alice")) {
			party = 1;
		}
		if(partyStr.equalsIgnoreCase("Bob")) {
			party = 2;
		}
		this.port = port;
	
		
	}
	
	
	// for compilation tests we don't need party/port
	public EmpParty() {
	}

	public String asString() {
		if(party == 1) {
			return "Alice";
		}
		if(party == 2) {
			return "Bob";
		}
		
		return "";
	}
	
	public int asInt() {
		return party;
	}
	
	public int getPort() {
		return port;
	}

	public void setPort(int port2) {
		port = port2;
		
	}

	
}
