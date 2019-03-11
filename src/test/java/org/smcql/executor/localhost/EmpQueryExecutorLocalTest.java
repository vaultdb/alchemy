package org.smcql.executor.localhost;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

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


public class EmpQueryExecutorLocalTest extends BaseTest {
  public List<WorkerConfiguration> workers;

  protected void setUp() throws Exception {
    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

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
  public void testJoin() throws Exception {
    String testName = "JoinCdiff";
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
    String distributedQuery =
        "WITH all_diagnoses AS ((SELECT patient_id, icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)), "
            + "all_medications AS ((SELECT patient_id FROM medications) UNION ALL (select patient_id FROM remote_medications)) "
            + "SELECT d.patient_id FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id AND icd9=\'008.45\' ORDER BY d.patient_id;";

    		System.out.println("Distributed query: " + distributedQuery);

    QueryTable expectedOutput = getExpectedOutput(testName, query, distributedQuery);

    testCase(testName, query, expectedOutput);
  }

  // TODO: George, please work on getting the code generator to build code for this
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  // TODO: need to check for dummy tag in generated code.  Recommend adding it to merge to zero out tuples that don't match icd9=414.01 in this case
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

    QueryTable observedOutput = exec.getOutput();
    //************ temp until we debug sorter in emp     **************//
    observedOutput.sort();
    
    assertEquals(expectedOutput, observedOutput);
    
  }
}
