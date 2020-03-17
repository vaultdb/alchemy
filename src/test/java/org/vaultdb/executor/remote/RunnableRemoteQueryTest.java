package org.vaultdb.executor.remote;

import java.util.List;
import java.util.logging.Level;
import org.vaultdb.BaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.EmpExecutor;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.executor.smc.ExecutionSegment;
import org.vaultdb.plan.SecureRelRoot;


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