package org.vaultdb.executor.smc.runnable;

import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.smc.ExecutionSegment;
import org.vaultdb.executor.smc.OperatorExecution;
import org.vaultdb.executor.smc.SecureQueryTable;

public interface SMCRunnable {

	public void sendInt(int toSend);
	
	public void sendTuple(Tuple toSend);
		
	public int getInt();
	
	public Tuple getTuple();
	
	
	public ExecutionSegment getSegment();
	
	public SecureQueryTable getOutput();
	
	public OperatorExecution getRootOperator();
	
}
