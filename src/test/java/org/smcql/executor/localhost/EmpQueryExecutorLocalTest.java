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
	  super.setUp();
	  
    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

  }

  public void testCountIcd9s() throws Exception {

    String query = "SELECT COUNT(DISTINCT major_icd9) FROM diagnoses";
    // to run in plaintext to verify our results
    String testName = "CountIcd9s";


    testCase(testName, query);
  }

  // TODO: Keith please work on getting this going
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  public void testJoin() throws Exception {
    String testName = "JoinCdiff";
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
    
    testCase(testName, query);
  }

  public void testFilterDistinct() throws Exception {
    String testName = "FilterDistinct";
    String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
    testCase(testName, query);
  }

  protected QueryTable getExpectedOutput(String testName, String query)
      throws Exception {
	  	
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
