package org.smcql.executor.remote;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.EmpExecutor;
import org.smcql.executor.SegmentExecutor;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.smc.ExecutionSegment;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;


public class RunnableRemoteQueryTest extends BaseTest {
	public String aWorkerId = null;
	public String bWorkerId = null;
	
	
	
	protected void setUp() throws Exception {
		
		  // TODO: test in remote setting by setting line in setup.global
		  // "distributed-eval-enabled=true"
		  
		  System.setProperty("smcql.location", "distributed");
		  super.setUp();

		
		
		ConnectionManager cm = ConnectionManager.getInstance();
		List<WorkerConfiguration> workers = cm.getWorkerConfigurations();
		
		if(workers.size() >= 2) {
			aWorkerId = workers.get(0).workerId;
			bWorkerId = workers.get(1).workerId;

		}
		
		
	}



	public void testComorbidity() throws Exception {
		String testName = "comorbidity";
		runTest(testName);
	}

	public void testCDiff() throws Exception {
		String testName = "cdiff";
		runTest(testName);
	}
	
	public void testAspirinCount() throws Exception {
		String testName = "aspirin-profile";
		runTest(testName);
	}


	private void runTest(String testName) throws Exception {


		if(!config.distributedEvaluationEnabled())
			return;

		
		config.resetCounters();
		
		String sql = super.readSQL(testName);
		
		
		logger.log(Level.INFO, "Parsing " + sql);
		
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		QueryCompiler qc = new QueryCompiler(secRoot, sql);
		
		List<ExecutionSegment> segments = qc.getSegments();
		ExecutionSegment segment = segments.get(0);
		logger.log(Level.INFO, "Segment has slice complement " + segment.sliceComplementSQL);
		logger.log(Level.INFO, "Segment has out schema " + segment.outSchema);
		logger.log(Level.INFO, "Have segment count " + segments.size());
		
		EmpExecutor exec = new EmpExecutor(qc, aWorkerId, bWorkerId);
		exec.run();
		
	    QueryTable results = exec.getOutput();
	    logger.log(Level.INFO, "output: " + results);
	    
	    QueryTable expected = getExpectedOutput(sql);
	    assertEquals(expected, results);
	}
}