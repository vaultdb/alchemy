package org.smcql.executor;

import org.smcql.codegen.QueryCompiler;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.smc.ExecutionSegment;
import org.smcql.executor.smc.SecureQueryTable;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;
import org.smcql.type.SecureRelRecordType;

import java.util.List;
import java.util.ListIterator;


// handles only 2 nodes, must contain at least one SecureStep in plan

public class EmpExecutor extends MPCExecutor {
	
	SegmentExecutor runner = null;
	QueryCompiler compiledPlan = null;
	private SecureRelRecordType lastSchema;
	private List<boolean[]> lastOutput;
	private QueryTable plainOutput;
	String queryId; 
	SecureRelRecordType outSchema;
	
	public EmpExecutor(QueryCompiler compiled, List<String> workers) throws Exception {
		runner = new SegmentExecutor(workers.get(0), workers.get(1));
		compiledPlan = compiled;
		queryId = compiledPlan.getQueryId();
		outSchema = compiledPlan.getPlan().getPlanRoot().getSchema();
	}

	public EmpExecutor(QueryCompiler compiled, String aWorkerId, String bWorkerId) throws Exception {
		runner = new SegmentExecutor(aWorkerId, bWorkerId);
		compiledPlan = compiled;
		queryId = compiledPlan.getQueryId();
		outSchema = compiledPlan.getPlan().getPlanRoot().getSchema();

	}
	
	public void run() {
		List<ExecutionSegment> segments = compiledPlan.getSegments();
		
		ExecutionStep root = compiledPlan.getRoot();
		
		for (SecureRelDataTypeField field : root.getExec().outSchema.getAttributes()) {
			if (field.getSecurityPolicy().equals(SecurityPolicy.Private)) {
				System.out.println("Exception: Private attribute " + field.getName() + " in out schema!");
				return;
			}
		}
		
		if(root instanceof PlaintextStep) {			
			try {
				plainOutput = runner.runPlain((PlaintextStep) root);
			} catch (Exception e) {
				System.out.println("Exception: No runnable execution step!");
			}
			return;
		}
		

		// iterate in reverse order to go bottom up
		ListIterator<ExecutionSegment> li = segments.listIterator(segments.size());

		try {
		
			while(li.hasPrevious()) { 
				ExecutionSegment segment = li.previous();
				 lastOutput = runner.runSecure(segment, queryId);
				 lastSchema = segment.outSchema;
			}
		}
		catch(Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}
	
	public QueryTable getOutput() throws Exception {
		if (lastOutput == null && plainOutput == null)
			return null;
		
		if(compiledPlan.getRoot() instanceof PlaintextStep) {
			return plainOutput;
		}
		
		boolean[] lhs = lastOutput.get(0);
		boolean[] rhs = lastOutput.get(1);
		boolean[] decrypted = new boolean[lhs.length];
		
		assert(lhs.length == rhs.length);
		
		for(int i = 0; i < lhs.length; ++i) {
			decrypted[i] = lhs[i] ^ rhs[i];
		}
		
		return new QueryTable( decrypted, outSchema);	
		
	}
	
	
}
