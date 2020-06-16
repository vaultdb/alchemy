package org.vaultdb.codegen.secure;

import org.apache.commons.io.FileUtils;

import org.vaultdb.BaseTest;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.Utilities;

import java.io.File;
import java.util.logging.Level;


// TODO: update this with working generated code when it is ready
public class GenerateSmcTpcHTest extends TpcHBaseTest {

  protected void setUp() throws Exception {
	  String dstDir = Utilities.getVaultDBRoot() + "/src/main/java/org/vaultdb/compiler/emp/generated/";
	  
	  // set up the new files
	  FileUtils.touch(new File(dstDir + "CountIcd9sCodegen.java"));
	  FileUtils.touch(new File(dstDir + "JoinCdiffCodegen.java"));
	  FileUtils.touch(new File(dstDir + "FilterDistinctCodegen.java"));

	  super.setUp();
  }

  public void testTpcHQuery1() throws Exception {
	  String testName = "TpcHQuery1";
	  String query = super.readSQL(1);


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

	  // testing the code generator alone for now, evaluate compiler separately
	  //qc.compileEmpCode();

    
	  String cwd = Utilities.getVaultDBRoot();
	  String expectedFile =
        cwd + "/src/test/java/org/vaultdb/codegen/secure/expected/" + testName + ".h";
	  logger.info("Generated: " + generatedFile);
	  logger.info("Expected: " + expectedFile);

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
