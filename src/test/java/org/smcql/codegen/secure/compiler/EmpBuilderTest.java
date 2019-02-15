package org.smcql.codegen.secure.compiler;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;

public class EmpBuilderTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}
	
	public void testCount() throws Exception {

		String query = "SELECT COUNT(DISTINCT icd9) FROM diagnoses";
		String distributedQuery = Utilities.getDistributedQuery(query);
		String testName = "count-icd9s";
		
		buildTest(testName, query);

	}

	
	protected void buildTest(String testName, String sql) throws Exception  {
		SystemConfiguration.getInstance().resetCounters();
		Logger logger = SystemConfiguration.getInstance().getLogger();
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
			
		QueryCompiler qc = new QueryCompiler(secRoot);
		
		ExecutionStep root = qc.getRoot();
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
	
		String empFile = qc.writeOutEmpFile();
		String empCode = qc.getEmpCode();
		
		//
		// TODO: Madhav, invoke builder chain and verify that it successfully creates binary 
		
		}
}
