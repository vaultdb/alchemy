package org.vaultdb.executor;

import java.util.List;

import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.db.data.QueryTable;

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
