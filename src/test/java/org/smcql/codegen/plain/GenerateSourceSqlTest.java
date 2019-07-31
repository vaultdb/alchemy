package org.smcql.codegen.plain;

import java.util.HashMap;
import java.util.Map;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;

public class GenerateSourceSqlTest extends BaseTest {
	
	protected void setUp() throws Exception {
		super.setUp();
	}
	
	public void testAsprinCount() throws Exception {
		Map<String, String> expected = new HashMap<String, String>();
		
		String scan0 = "SELECT patient_id, timestamp_\n"
				 + "FROM (SELECT patient_id, icd9, timestamp_\n"
				 + "FROM mi_cohort_diagnoses) AS t\n"
				 + "WHERE icd9 LIKE '414%'\n"
				 + "ORDER BY patient_id, timestamp_";
		
		String scan4 = "SELECT patient_id, timestamp_\n"
				 + "FROM (SELECT patient_id, medication, timestamp_\n"
				 + "FROM mi_cohort_medications) AS t\n"
				 + "WHERE LOWER(medication) LIKE '%aspirin%'\n"
				 + "ORDER BY patient_id, timestamp_";
		
		
		expected.put("SeqScan4", scan4);
		expected.put("SeqScan0", scan0);
		runTest("aspirin-count", expected);		
	}
	
	public void testCDiff() throws Exception {		
		Map<String, String> expected = new HashMap<String, String>();
		String scan0 = "SELECT patient_id, timestamp_, EXTRACT(EPOCH FROM timestamp_) / 86400 AS $2\n"
				 + "FROM cdiff_cohort_diagnoses\n"
				 + "WHERE icd9 = '008.45'\n"
				 + "ORDER BY patient_id, timestamp_";
		
		expected.put("SeqScan0", scan0);
		runTest("cdiff", expected);
	}

	public void testComorbidity() throws Exception {		
		Map<String, String> expected = new HashMap<String, String>();
		expected.put("SeqScan0", "SELECT major_icd9\nFROM cdiff_cohort_diagnoses");
		runTest("comorbidity", expected);
	}
	
	public void runTest(String testName, Map<String, String> expected) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		
		String sql = super.readSQL(testName);
		System.out.println("Parsing " + sql);
		
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		QueryCompiler qc = new QueryCompiler(secRoot, sql);
		
		Map<String, String> steps = qc.getSQLCode();
		assertEquals(expected.size(), steps.size());
		
		for (String functionName : steps.keySet()) {
			String sqlInput = steps.get(functionName);
			assertEquals(expected.get(functionName), sqlInput);
		}
		
	}
}
