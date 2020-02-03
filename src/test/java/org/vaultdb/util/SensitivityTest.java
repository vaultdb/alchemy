package org.vaultdb.util;

import java.util.HashMap;
import java.util.Map;

import org.vaultdb.BaseTest;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.plan.SecureRelRoot;

public class SensitivityTest extends BaseTest{
	
	protected void setUp() throws Exception {
		super.setUp();
	}
	  
	public void testCountIcd9s() throws Exception {
		String query = "SELECT major_icd9,COUNT(*) FROM diagnoses GROUP BY major_icd9";
	    String testName = "CountIcd9s";

	    int expectedSensitivity = 1;
	    testCase(testName, query, new HashMap<String, Integer>(), expectedSensitivity);	  
	}

	public void testJoin() throws Exception {
		String testName = "JoinCdiff";
	    String query =
	            "SELECT  d.patient_id "
	    		+"FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id "
	            + "WHERE icd9=\'008.45\' ";

	    int expectedSensitivity = 5;
	    HashMap<String, Integer> maxFrequencies = new HashMap<String, Integer>();
	    maxFrequencies.put("patient_id", 5); //just for testing; in practice, max frequency for primary keys is 1
	    
	    testCase(testName, query, maxFrequencies, expectedSensitivity);	  
	}

	public void testFilterDistinct() throws Exception {
		String testName = "FilterDistinct";
	    String query = "SELECT DISTINCT  patient_id FROM diagnoses WHERE icd9 = \'414.01\' ORDER BY patient_id";

	    int expectedSensitivity = 1;
	    testCase(testName, query, new HashMap<String, Integer>(), expectedSensitivity);	 
	}  
	
	protected void testCase(String testName, String sql, Map<String, Integer> maxFrequencies, int expectedSensitivity) throws Exception {
	    SystemConfiguration.getInstance().resetCounters();
	    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	    int sensitivity = Utilities.getSensitivity(secRoot.getPlanRoot(), maxFrequencies);
	    
	    assertEquals(expectedSensitivity, sensitivity);
	}
}
