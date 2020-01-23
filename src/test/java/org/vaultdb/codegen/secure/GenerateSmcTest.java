package org.vaultdb.codegen.secure;

import java.io.File;
import java.util.logging.Level;
import org.apache.commons.io.FileUtils;
import org.vaultdb.BaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.Utilities;


// TODO: update this with working generated code when it is ready
public class GenerateSmcTest extends BaseTest {

  protected void setUp() throws Exception {
	  super.setUp();
  }

  public void testCountIcd9s() throws Exception {
	  String query = "SELECT COUNT(DISTINCT major_icd9) FROM diagnoses";
	  String testName = "CountIcd9sCodegen";


	  testCase(testName, query);
  }

  public void testJoin() throws Exception {
	                     
	  String testName = "JoinCdiffCodegen";
	  String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
      testCase(testName, query);
  }

  public void testFilterDistinct() throws Exception {  
	   String testName = "FilterDistinctCodegen";
	   String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
	   testCase(testName, query);
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
        cwd + "/src/test/java/org/vaultdb/codegen/secure/expected/" + testName + ".h";
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

	  assertTrue("The jni wrappers " + generatedFile + "\n" + expectedFile + "\n differ!", FileUtils.contentEquals(generated, expected));
	  EmpJniUtilities.cleanEmpCode(testName);
  }
}
