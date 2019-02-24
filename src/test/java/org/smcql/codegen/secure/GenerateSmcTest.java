package org.smcql.codegen.secure;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.io.FileUtils;
import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.EmpExecutor;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CommandOutput;
import org.smcql.util.Utilities;

public class GenerateSmcTest extends BaseTest {
  public ArrayList<String> workerIds;

  protected void setUp() throws Exception {
    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    List<WorkerConfiguration> workers = cm.getWorkerConfigurations();
    workerIds = new ArrayList<String>();

    if (workers.size() >= 2) {
      workerIds.add(workers.get(0).workerId);
      workerIds.add(workers.get(1).workerId);
    }
  }

  public void testCountIcd9s() throws Exception {

    String query = "SELECT COUNT(DISTINCT major_icd9) FROM diagnoses";
    // to run in plaintext to verify our results
    String distributedQuery = "WITH all_diagnoses AS ((SELECT major_icd9 FROM diagnoses) UNION ALL (SELECT major_icd9 FROM remote_diagnoses)) SELECT COUNT(DISTINCT major_icd9) FROM all_diagnoses;";
    String testName = "CountIcd9s";

    QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);

    testCase(testName, query, expectedOutput);
  }

  // TODO: Keith please work on getting this going
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  // make sure to create a file in expected directory called "join-cdiff-emp.cpp"
  public void testJoin() throws Exception {
    String testName = "JoinCdiff";
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
    String distributedQuery =
        "WITH all_diagnoses AS ((SELECT patient_id, icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)), "
            + "all_medications AS ((SELECT patient_id FROM medications) UNION ALL (select patient_id FROM remote_medications)) "
            + "SELECT DISTINCT d.patient_id FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id AND icd9=\'008.45\';";

    //		String distributedQuery = Utilities.getDistributedQuery(query);
    //		System.out.println("Distributed query: " + distributedQuery);

    QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);

    testCase(testName, query, expectedOutput);
  }

  // TODO: George, please work on getting the code generator to build code for this
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  // make sure to create a file in expected directory called "filter-distinct-emp.cpp"

  public void testFilterDistinct() throws Exception {
    String testName = "FilterDistinct";
    String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
    		String distributedQuery = "WITH all_diagnoses AS ((SELECT patient_id,icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)) " +
    				"SELECT DISTINCT patient_id FROM all_diagnoses WHERE icd9 = \'414.01\'";

    QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);
    testCase(testName, query, expectedOutput);
  }

  protected QueryTable getExpectedOutput(String testName, String query, String distributedQuery)
      throws Exception {
    String aliceId = ConnectionManager.getInstance().getAlice();
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);

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

    String cwd = Utilities.getSMCQLRoot();
    String expectedFile =
        cwd + "/src/test/java/org/smcql/codegen/secure/expected/" + testName + ".h";
    System.out.println("Generated: " + generatedFile);
    System.out.println("Expected: " + expectedFile);

    int exitCode = qc.compileEmpCode();
    assertEquals(0, exitCode);

    /*File generated = new File(generatedFile);
    File expected = new File(expectedFile);
    
    // TODO: re-add this before shipping
    //assertTrue("The emp code differs!", FileUtils.contentEquals(generated, expected));

    // check the jni wrappers  ".h" --> ".java"
    generatedFile = generatedFile.substring(0, generatedFile.length() - 1);
    generatedFile += "java";
    generated = new File(generatedFile);

    expectedFile = expectedFile.substring(0, expectedFile.length() - 1);
    expectedFile += "java_"; // underscore to prevent compiler from complaining about package path
    expected = new File(expectedFile);

    // TODO: re-add these before shipping
    //assertTrue("The jni wrappers differ!", FileUtils.contentEquals(generated, expected));
	*/
    
    EmpExecutor exec = new EmpExecutor(qc, workerIds);
    exec.run();

    QueryTable observedOutput = exec.getOutput();
    assertEquals(expectedOutput, observedOutput);
  }
}
