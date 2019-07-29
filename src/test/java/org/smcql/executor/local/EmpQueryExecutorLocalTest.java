package org.smcql.executor.local;

import java.util.ArrayList;
import java.util.List;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.EmpExecutor;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.FileUtilities;
import org.smcql.util.Utilities;


public class EmpQueryExecutorLocalTest extends BaseTest {
  public List<WorkerConfiguration> workers;

  protected void setUp() throws Exception {
    super.setUp();
    
    ConnectionManager cm = ConnectionManager.getInstance();
    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

  }

  public void testCountIcd9s() throws Exception {

	
    String query = "SELECT major_icd9,COUNT(*) FROM diagnoses GROUP BY major_icd9";
    // to run in plaintext to verify our results
    String testName = "CountIcd9s";


    testCase(testName, query);
  }

  public void testJoin() throws Exception {
	  /* JR: Temporarily commented out
	   * 
    String testName = "JoinCdiff";
    String query =
            "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";

    testCase(testName, query); 
    	   */

  }

  public void testFilterDistinct() throws Exception {
	String testName = "FilterDistinct";
    String query = "SELECT DISTINCT  patient_id FROM diagnoses WHERE icd9 = \'414.01\' ORDER BY patient_id";

    // query to test distinct - currently crashes
    // String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE encounter_id = \'4\' ";

    testCase(testName, query);
  }


  protected void testCase(String testName, String sql) throws Exception {
    SystemConfiguration.getInstance().resetCounters();
    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);



    System.out.println("Initial schema: " + secRoot.getPlanRoot().getSchema() );
    QueryCompiler qc = new QueryCompiler(secRoot);
    qc.writeOutEmpFile();

    String empTarget = Utilities.getCodeGenTarget() + "/" + testName + ".h";
    String jniTarget = Utilities.getCodeGenTarget() + "/" + testName + ".java";

    assertTrue(FileUtilities.fileExists(empTarget));
    assertTrue(FileUtilities.fileExists(jniTarget));

    EmpExecutor exec = new EmpExecutor(qc);
    exec.run();

    QueryTable expectedOutput = getExpectedOutput(sql);
    QueryTable observedOutput = exec.getOutput();

    logger.info("Observed output: \n" + observedOutput);
    logger.info("Expected output: \n" + expectedOutput);

    assertEquals(expectedOutput.tupleCount(), observedOutput.tupleCount());
    assertEquals(expectedOutput, observedOutput);

  }
}
