package org.vaultdb.executor.step;

import java.util.List;

import org.vaultdb.codegen.CodeGenerator;
import org.vaultdb.executor.config.RunConfig;
import org.vaultdb.executor.smc.OperatorExecution;
import org.vaultdb.plan.operator.Operator;
import org.vaultdb.type.SecureRelRecordType;

public interface ExecutionStep {
	 
	
	// generates a method for MPC
	public String generate() throws Exception;
		
	public String getPackageName();

	public String getFunctionName();
	
	public ExecutionStep getParent();

	public ExecutionStep getChild(int idx);
	
	public SecureRelRecordType getInSchema();
	
	public SecureRelRecordType getSchema();

	public SecureRelRecordType getSchema(boolean forSecureLeaf);

	
	public RunConfig getRunConfig();
	
	public void setParent(ExecutionStep e);
	
	public String toString();
	
	public Operator getSourceOperator();
	
	public CodeGenerator getCodeGenerator();
	
	public List<ExecutionStep> getChildren();
	
	public boolean visited(); // check if visited
	
	public void visit(); // mark step as executed
	
	public void setHostname(String host); // where it will be run, Alice/generator if SecureStep

	public String printTree();
	
	public OperatorExecution getExec();
}
