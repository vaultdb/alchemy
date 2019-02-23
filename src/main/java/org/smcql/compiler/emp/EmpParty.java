package org.smcql.compiler.emp;

import org.smcql.config.SystemConfiguration;

// simple party definition
public class EmpParty {
	
	int party = 0; // agnostic, not alice or bob
	int port = 54321;
	
	public EmpParty(int party) throws Exception {
		this.party = party;
		getEmpPort();
		
		
	}
	
	public EmpParty(String partyStr) throws Exception {

		if(partyStr.equalsIgnoreCase("Alice")) {
			party = 1;
		}
		if(partyStr.equalsIgnoreCase("Bob")) {
			party = 2;
		}
	
		getEmpPort();
	}
	
	
	private void getEmpPort() throws Exception {
		// try local source
				String empPort = SystemConfiguration.getInstance().getProperty("emp-port");
				if(empPort != null && empPort != "") {
					port = Integer.parseInt(empPort); // TODO: check if it is numeric
				}
				else {
					// handle remote case
				   port = Integer.parseInt(System.getProperty("emp-port"));
				}
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

	
}
