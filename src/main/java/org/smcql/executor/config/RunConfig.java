package org.smcql.executor.config;

import java.io.Serializable;


// parts of config that are agnostic to Alice/Bob
public class RunConfig implements Serializable {

	/**
	 * 
	 */
	
	private static final long serialVersionUID = -6316868926248404447L;

	
	ExecutionMode executionMode;
	

	public int port = 54321;
	public String host = "localhost"; // location of generator 



	public RunConfig() {
		
	}
	
	public RunConfig(int aPort,  String aHost) {
		port = aPort;
		host = aHost;
	}
	
	
	@Override
	public boolean equals(Object o) {
		if(o instanceof RunConfig) {
			RunConfig r = (RunConfig) o;
			if(this.port == r.port 
					&& this.host.equals(r.host)) {
				return true;
			}
		}
		
		return false;
	}
		
	@Override
	public String toString() {
		return host + ":" + port;
	}
	
}
