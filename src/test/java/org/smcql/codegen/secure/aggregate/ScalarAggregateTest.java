package org.smcql.codegen.secure.aggregate;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.EmpExecutor;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

import java.util.ArrayList;
import java.util.List;


public class ScalarAggregateTest extends BaseTest {
	
  public List<WorkerConfiguration> workers;

  protected void setUp() throws Exception {
    super.setUp();

    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

  }

  // TODO: Create test queries from health database for Groupby: Count & Sum, Splitscalar: Count & Sum
  // TODO: Fix race condition, so queries will pass on single run. Currently they require 2 runs to pass


  public void testScalarMax() throws Exception {

    String testName = "ScalarMax";

    String query = "SELECT Max(month_id) FROM diagnoses";
    testQuery(testName, query);
  }

  public void testScalarMin() throws Exception {

    String testName = "ScalarMin";

    String query = "SELECT Min(month_id) FROM diagnoses";
    testQuery(testName, query);
  }

  public void testScalarSum() throws Exception {

    String testName = "testScalarSum";

    String query = "SELECT Sum(month_id) FROM diagnoses";
    testQuery(testName, query);
  }


  public void testScalarMultiAgg() throws Exception {

    String testName = "ScalarMultiAggregate";

    String query = "SELECT Min(month_id), Max(month_id), Sum(month_id) FROM diagnoses";
    testQuery(testName, query);
  }




  public void testSplitscalarCount() throws Exception {

    String testName = "SplitscalarCount";

    // String query = "";
    // testQuery(testName, query);
  }


  public void testSplitscalarSum() throws Exception {

    String testName = "testSplitscalarSum";

    // String query = "";
    // testQuery(testName, query);
  }



}
