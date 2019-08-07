package org.vaultdb.executor.smc;

import java.io.Serializable;

import org.vaultdb.codegen.smc.operator.SecureOperator;
import org.vaultdb.config.SystemConfiguration.Party;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.executor.step.PlaintextStep;
import org.vaultdb.executor.step.SecureStep;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;



// basically a stripped down version of ExecutionStep for serialization
public class OperatorExecution implements Comparable<OperatorExecution>, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 6565213527478140219L;
	public String packageName;
	public SecureRelRecordType outSchema;
	public OperatorExecution parent = null;
	public OperatorExecution lhsChild, rhsChild;  // may be root of another segment
	public ExecutionSegment parentSegment = null; // pointer to segment for SMCConfig 
	public byte[] byteCode; // compiled .class for this step
	public boolean[] output;
	
	 // for merge case
	protected String sourceSQL = null;
	
	
	public OperatorExecution() {
		
	}
	
	
	public OperatorExecution(SecureStep s) throws Exception {
		packageName = s.getPackageName();
		outSchema = s.getSchema(); //change this
		
		SecureOperator sec = s.getSourceOperator().getSecureOperator();
		
		lhsChild = getChild(s, 0);
		rhsChild = getChild(s, 1);
		
		if(lhsChild != null) 
			lhsChild.parent = this;
		
		if(rhsChild != null) 
			rhsChild.parent = this;
		
		/* this will always fail in emp land
		 * try {
			byteCode = Utilities.readGeneratedClassFile(packageName);
			
			for (ProcessingStep p : processingSteps) {
				String name = packageName + "." + p.getProcessName();
				p.setByteCode(Utilities.readGeneratedClassFile(name));
			}
		} catch(Exception e) {
			// do nothing
		}*/
		
	}
	
	
	public OperatorExecution(PlaintextStep s) {
		packageName = s.getPackageName();
		outSchema = s.getSchema();
		
		
		lhsChild = getChild(s, 0);
		rhsChild = getChild(s, 1);
		
		if(lhsChild != null) 
			lhsChild.parent = this;
		
		if(rhsChild != null) 
			rhsChild.parent = this;
	}


	private OperatorExecution getChild(ExecutionStep src, int idx) {
		ExecutionStep e = src.getChild(idx);
		if(e instanceof SecureStep)
			return ((SecureStep) e).getExec();
		return null;
	}
	@Override
	public int compareTo(OperatorExecution o) {
		return this.packageName.compareTo(o.packageName);
	}
	
	
	public Party getParty() {
		if(parentSegment != null)
			return parentSegment.party;
		return null;
	}
	
	public String getWorkerId() {
		if(parentSegment != null) 
			return parentSegment.workerId;
		return null;
	}

	@Override
	public String toString() {
		String lhsChildStr = (lhsChild != null) ? lhsChild.packageName : "null";
		String rhsChildStr = (rhsChild != null) ? rhsChild.packageName : "null";

		String ret = packageName + "(" + lhsChildStr + "," + rhsChildStr + ") " + getParty() + ", " + getWorkerId();
		if(getSourceSQL() != null)
			ret += "source: " + getSourceSQL();
		
		return ret;
	}


	public String getSourceSQL() {
		return sourceSQL;
	}


	public void setSourceSQL(String sourceSql) throws Exception {
		this.sourceSQL = sourceSql;
	}

	public void updatePrivacyBudget() throws Exception {
		for (SecureRelDataTypeField f : outSchema.getAttributes()) {
			String key = f.getStoredTable() + "." + f.getName();
		}
	}
	
}
