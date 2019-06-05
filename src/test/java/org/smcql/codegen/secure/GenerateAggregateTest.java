package org.smcql.codegen.secure;

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


public class GenerateAggregateTest extends BaseTest {
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

    String testName = "testScalarMax";

    String query = "SELECT Max(month_id) FROM diagnoses";
    testCase(testName, query);
  }

  public void testScalarMin() throws Exception {

    String testName = "testScalarMin";

    String query = "SELECT Min(month_id) FROM diagnoses";
    testCase(testName, query);
  }

  public void testScalarSum() throws Exception {

    String testName = "testScalarSum";

    String query = "SELECT Sum(month_id) FROM diagnoses";
    testCase(testName, query);
  }


  public void testScalarMultiAgg() throws Exception {

    String testName = "testScalarMultiAggregate";

    String query = "SELECT Min(month_id), Max(month_id), Sum(month_id) FROM diagnoses";
    testCase(testName, query);
  }




  public void testGroupbyCount() throws Exception {

    String testName = "testGroupbyCount";

    // String query = "SELECT COUNT(month_id) FROM diagnoses GROUP BY major_icd9";
    String query = "SELECT major_icd9, Count(*) FROM diagnoses GROUP BY major_icd9";
    testCase(testName, query);
  }

  public void testGroupbySum() throws Exception {

    String testName = "testGroupbySum";
    // TODO: Get Sum( <any integer input> ) to compile. Some options from diagnoses are: patient_id, site, month

    String query = "SELECT Sum(month_id) FROM diagnoses GROUP BY major_icd9";
    testCase(testName, query);
  }


  public void testSplitscalarCount() throws Exception {

    String testName = "testSplitscalarCount";

    // String query = "";
    // testCase(testName, query);
  }


  public void testSplitscalarSum() throws Exception {

    String testName = "testSplitscalarSum";

    // String query = "";
    // testCase(testName, query);
  }


  protected QueryTable getExpectedOutput(String testName, String query) throws Exception {

    String unionedId = ConnectionManager.getInstance().getUnioned();
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
    return SqlQueryExecutor.query(query, outSchema, unionedId);
  }




  protected void testCase(String testName, String sql) throws Exception {
    SystemConfiguration.getInstance().resetCounters();
    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);

    System.out.println("Initial schema: " + secRoot.getPlanRoot().getSchema() );
    QueryCompiler qc = new QueryCompiler(secRoot);
    qc.writeOutEmpFile();

    String empTarget = Utilities.getCodeGenTarget() + "/" + testName + ".h";
    String jniTarget = Utilities.getCodeGenTarget() + "/" + testName + ".java";

    assertTrue(FileUtils.fileExists(empTarget));
    assertTrue(FileUtils.fileExists(jniTarget));

    EmpExecutor exec = new EmpExecutor(qc);
    exec.run();

    QueryTable expectedOutput = getExpectedOutput(testName, sql);
    QueryTable observedOutput = exec.getOutput();

    logger.info("Observed output: \n" + observedOutput);
    logger.info("Expected output: \n" + expectedOutput);

    assertEquals(expectedOutput.tupleCount(), observedOutput.tupleCount());
    assertEquals(expectedOutput, observedOutput);

  }
}
