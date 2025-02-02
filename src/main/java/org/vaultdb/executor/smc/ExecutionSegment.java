package org.vaultdb.executor.smc;

import java.io.Serializable;
import java.util.List;

import org.vaultdb.config.SystemConfiguration.Party;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.config.ExecutionMode;
import org.vaultdb.executor.config.RunConfig;
import org.vaultdb.plan.slice.SliceKeyDefinition;
import org.vaultdb.type.SecureRelRecordType;


// sequence of generated operators for execution (no sync points with other ops / hosts)
public class ExecutionSegment implements Serializable {

	public OperatorExecution rootNode;
	
	public SliceKeyDefinition sliceSpec;
	
	// replacing SMCConfig
	public RunConfig runConf;
	public String workerId;
	public Party party = null;
	public int port;
	
	// for sliced merge
	public SecureRelRecordType outSchema;

	public boolean isPlanRoot = false;
	
	public String sliceComplementSQL = null;
	public List<Tuple> sliceValues;
	public List<Tuple> complementValues;
	String className;
	public String empCode;
	public String jniCode;
	public ExecutionMode executionMode;
	
	
	public void checkInit() throws Exception {
		if(party == null || workerId == null) {
			throw new Exception("Parent segment uninitialized!");
		}
		checkInitHelper(rootNode);
	}

	public void checkInitHelper(OperatorExecution op) throws Exception {
		if(op == null || op.parentSegment != this) {
			return;
		}
		
		if(op.parentSegment == null) {
			throw new Exception("Segment uninitialized for " + op);
		}
		
		
		if(op.getWorkerId() == null || op.getParty() == null) {
			throw new Exception("Bad configuration for " + op);
		}
		
		checkInitHelper(op.lhsChild);
		checkInitHelper(op.rhsChild);
		
	}

	// for use after a single slice execution to prepare for next one
	public void resetOutput() {
		resetOutputHelper(rootNode);
	}
	
	private void resetOutputHelper(OperatorExecution op) {
		if(op == null || op.parentSegment != this)
			return;
		
		op.output = null;
		resetOutputHelper(op.lhsChild);
		resetOutputHelper(op.rhsChild);

		
	}
	
}
