package org.smcql.codegen.secure;

import java.io.File;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.io.FileUtils;
import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

public class GenerateSmcTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}
	
/*	public void testComorbidity() throws Exception {
		testCase("comorbidity");
	}
	

	public void testCDiff() throws Exception {
		testCase("cdiff");
	}

	public void testAspirinCount() throws Exception {
		testCase("aspirin-count");
	}*/
	
	public void testCount() throws Exception {
		
		String query = "SELECT COUNT(DISTINCT icd9) FROM diagnoses";
		// to run in plaintext to verify our results
		String distributedQuery = "WITH all_diagnoses AS ((SELECT icd9 FROM diagnoses) UNION ALL (SELECT icd9 FROM remote_diagnoses)) SELECT COUNT(DISTINCT icd9) FROM all_diagnoses;";
		String testName = "count-icd9s";

		QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);
		
		testCase(testName, query, expectedOutput);
	}
	
	
	// TODO: Keith please work on getting this going
	// use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test 
	// for guidance
	// make sure to create a file in expected directory called "join-cdiff-emp.cpp"
	public void testJoin() throws Exception {
		String testName = "join-cdiff";
		String query = "SELECT DISTINCT d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE d.icd9 = \'008.45\'";
		String distributedQuery = "WITH all_diagnoses AS ((SELECT patient_id, icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)), " + 
		    "all_medications AS ((SELECT patient_id FROM medications) UNION ALL (select patient_id FROM remote_medications)) " +
		     "SELECT DISTINCT d.patient_id FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\';";

	
		System.out.println("Distributed query: " + distributedQuery);
		
		QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);
	
		testCase(testName, query, expectedOutput);	
	}
	
	
	
	// TODO: George, please work on getting the code generator to build code for this
	// use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test 
	// for guidance
	// make sure to create a file in expected directory called "filter-distinct-emp.cpp"
		
	public void testFilterDistinct() throws Exception {
		String testName = "filter-distinct";
		String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
		String distributedQuery = "WITH all_diagnoses AS ((SELECT patient_id,icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)) " +
				"SELECT DISTINCT patient_id FROM all_diagnoses WHERE icd9 = \'414.01\'";
		
		QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);
		testCase(testName, query, expectedOutput);	
	
	}

	protected QueryTable getExpectedOutput(String testName, String query, String distributedQuery) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
		SecureRelRoot secRoot = new SecureRelRoot(testName, query);
		SecureRelRecordType outSchema = secRoot.getPlanRoot().getSchema();
	
		return SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
	
	}
	
	protected void testCase(String testName, String sql, QueryTable expectedOutput) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		Logger logger = SystemConfiguration.getInstance().getLogger();
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
			
		QueryCompiler qc = new QueryCompiler(secRoot);
		
		ExecutionStep root = qc.getRoot();
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		
	
		String generatedFile = qc.writeOutEmpFile();
		
		String cwd = System.getProperty("user.dir");
		String expectedFile = cwd + "/src/test/java/org/smcql/codegen/secure/expected/" + testName + "-emp.cpp";
		System.out.println("Generated: " + generatedFile);
		System.out.println("Expected: " + expectedFile);
		
	
		
		File generated = new File(generatedFile);
		File expected = new File(expectedFile);
		assertTrue("The files differ!", FileUtils.contentEquals(generated, expected));
		
		// TODO: run code compiler and executor here
		// TODO: collect results as a SecureQueryTable
		
		QueryTable observedOutput = null;  // Johes: please work on populating this
		assertEquals(expectedOutput, observedOutput);
		
	
		
	}
	
}
