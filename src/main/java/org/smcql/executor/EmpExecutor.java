package org.smcql.executor;

import org.smcql.codegen.QueryCompiler;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.smc.ExecutionSegment;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.type.SecureRelRecordType;

import java.util.List;
import java.util.ListIterator;


// handles only 2 nodes, must contain at least one SecureStep in plan

public class EmpExecutor extends MPCExecutor {
	
	SegmentExecutor runner = null;
	QueryCompiler compiledPlan = null;
	private QueryTable lastOutput;
	private QueryTable plainOutput;
	String queryId = null; 
	SecureRelRecordType outSchema;
	int exitCode = 0;
	
	public EmpExecutor(QueryCompiler compiled) throws Exception {
		ConnectionManager cm = ConnectionManager.getInstance();
		runner = new SegmentExecutor(cm.getAlice(), cm.getBob());
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
	
	public void runForTesting() throws Exception {
		List<ExecutionSegment> segments = compiledPlan.getSegments();
		ExecutionStep root = compiledPlan.getRoot();
		
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

		while(li.hasPrevious()) { 
			ExecutionSegment segment = li.previous();
			runner.setQueryCompiler(compiledPlan);
			lastOutput = runner.runSecure(segment, queryId);
			outSchema = segment.outSchema;
		}
	}
	
	public void run()  {
		List<ExecutionSegment> segments = compiledPlan.getSegments();
		
		ExecutionStep root = compiledPlan.getRoot();

		/*
		for (SecureRelDataTypeField field : root.getExec().outSchema.getAttributes()) {
			if (field.getSecurityPolicy().equals(SecurityPolicy.Private)) {
				System.out.println("Exception: Private attribute " + field.getName() + " in out schema!");
				return;
			}
		}
		*/
		
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
				runner.setQueryCompiler(compiledPlan);
				lastOutput = runner.runSecure(segment, queryId);
				outSchema = segment.outSchema;
			}
		}
		catch(Exception e) {
			e.printStackTrace();
			exitCode = -1;
		}
	}
	
	
	public int getExitCode() {
		return exitCode;
	}
	
	public QueryTable getOutput() throws Exception {
		if (lastOutput == null && plainOutput == null)
			return null;
		
		if(compiledPlan.getRoot() instanceof PlaintextStep) {
			return plainOutput;
		}
		
		return lastOutput;
		
		
	}
	
	
}
