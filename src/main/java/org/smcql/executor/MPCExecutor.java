package org.smcql.executor;

import java.util.List;

import org.smcql.codegen.QueryCompiler;
import org.smcql.db.data.QueryTable;

public abstract  class MPCExecutor implements Runnable {
	
	public MPCExecutor() {
		
	}
	
	 public   MPCExecutor(QueryCompiler qc, List<String> parties) throws Exception {
		 
	 }
	 
	public void run() {
			
		}
	
	public QueryTable getOutput() throws Exception {
		 return null;
	 }
	 

}
