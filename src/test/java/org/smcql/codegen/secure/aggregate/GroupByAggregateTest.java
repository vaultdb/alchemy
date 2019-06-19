package org.smcql.codegen.secure.aggregate;

import java.util.ArrayList;
import java.util.List;

import org.smcql.BaseTest;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.util.Utilities;

public class GroupByAggregateTest extends BaseTest {
	
	  public List<WorkerConfiguration> workers;

	  protected void setUp() throws Exception {
	    super.setUp();

	    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
	    System.setProperty("smcql.setup", setupFile);
	    ConnectionManager cm = ConnectionManager.getInstance();
	    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

	  }

	  
	  public void testDistinct() throws Exception {
		  String testName = "Distinct";
		  String query = "SELECT DISTINCT major_icd9 FROM diagnoses";
		  
		  testQuery(testName, query);
	  }

	  public void testGroupByCount() throws Exception {

		    String testName = "GroupByCount";

		    // String query = "SELECT COUNT(month_id) FROM diagnoses GROUP BY major_icd9";
		    String query = "SELECT major_icd9, COUNT(*) FROM diagnoses GROUP BY major_icd9";
		    testQuery(testName, query);
		  }

		  public void testGroupBySum() throws Exception {

		    String testName = "GroupBySum";
		    // TODO: Get Sum( <any integer input> ) to compile. Some options from diagnoses are: patient_id, site, month

		    String query = "SELECT major_icd9, SUM(month_id) FROM diagnoses GROUP BY major_icd9";
		    testQuery(testName, query);
		  }

		  public void testGroupBySumAlone() throws Exception {

			    String testName = "GroupBySum";
			    // TODO: Get Sum( <any integer input> ) to compile. Some options from diagnoses are: patient_id, site, month

			    String query = "SELECT SUM(month_id) FROM diagnoses GROUP BY major_icd9";
			    testQuery(testName, query);
			  }




}
