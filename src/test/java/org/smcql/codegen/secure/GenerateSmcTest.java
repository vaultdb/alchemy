package org.smcql.codegen.secure;

import java.io.File;
import java.util.logging.Level;
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
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.Utilities;

public class GenerateSmcTest extends BaseTest {

  protected void setUp() throws Exception {
	  super.setUp();
  }

  public void testCountIcd9s() throws Exception {

    String query = "SELECT COUNT(DISTINCT major_icd9) FROM diagnoses";
    String testName = "CountIcd9s";


    testCase(testName, query);
  }

  // TODO: Keith please work on getting this going
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  // make sure to create a file in expected directory called "join-cdiff-emp.cpp"
  public void testJoin() throws Exception {
    String testName = "JoinCdiff";
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
       testCase(testName, query);
  }

  // TODO: George, please work on getting the code generator to build code for this
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  // make sure to create a file in expected directory called "filter-distinct-emp.cpp"

  public void testFilterDistinct() throws Exception {
    String testName = "FilterDistinct";
    String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
    testCase(testName, query);
  }

  protected QueryTable getExpectedOutput(String testName, String query, String distributedQuery)
      throws Exception {
    String aliceId = ConnectionManager.getInstance().getAlice();
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);

    return SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
  }

  protected void testCase(String testName, String sql) throws Exception {

	EmpJniUtilities.cleanEmpCode(testName);
    SystemConfiguration.getInstance().resetCounters();
     
    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);

    QueryCompiler qc = new QueryCompiler(secRoot);

    ExecutionStep root = qc.getRoot();
    String testTree = root.printTree();
    logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);

    String generatedFile = qc.writeOutEmpFile();
    qc.compileEmpCode();
    

    
    String cwd = Utilities.getSMCQLRoot();
    String expectedFile =
        cwd + "/src/test/java/org/smcql/codegen/secure/expected/" + testName + ".h";
    System.out.println("Generated: " + generatedFile);
    System.out.println("Expected: " + expectedFile);

    File generated = new File(generatedFile);
    File expected = new File(expectedFile);
    
    assertTrue("The emp code differs!", FileUtils.contentEquals(generated, expected));

    // check the jni wrappers  ".h" --> ".java"
    generatedFile = generatedFile.substring(0, generatedFile.length() - 1);
    generatedFile += "java";
    generated = new File(generatedFile);

    expectedFile = expectedFile.substring(0, expectedFile.length() - 1);
    expectedFile += "java_"; // underscore to prevent compiler from complaining about package path
    expected = new File(expectedFile);

    assertTrue("The jni wrappers differ!", FileUtils.contentEquals(generated, expected));
	
  }
}
