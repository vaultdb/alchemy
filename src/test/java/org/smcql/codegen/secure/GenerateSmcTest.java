package org.smcql.codegen.secure;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.smcql.BaseTest;
import org.smcql.codegen.CodeCompiler;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;

public class GenerateSmcTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}
	
/*	public void testComorbidity() throws Exception {
		testCase("comorbidity");
	}
	

	public void testCDiff() throws Exception {
		testCase("cdiff");
	}

	public void testAspirinCount() throws Exception {
		testCase("aspirin-count");
	}*/
	
	public void testCount() throws Exception {
		testCase("count", "SELECT COUNT(DISTINCT icd9) FROM diagnoses");
	}
	
/*	protected void testCase(String testName) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		//compiles code to a couple of cpp files that will be sent to alice & bob for execution
		CodeCompiler cc = new CodeCompiler(secRoot);
		cc.compile();
		cc.writeToDisk(testName + ".cpp", Utilities.getCodeGenTarget() + "/" + testName);
		System.out.println("Query: " + testName.toUpperCase());
		System.out.println("\nTree:\n" + cc.getTree());
		System.out.println("\nSource SQL:\n" + cc.getSourceSQL());
		System.out.println("\nEMP Code:\n" + cc.getEmpCode());
		
		testName = testName.replace("-", "_");
		String expectedDir = Utilities.getSMCQLRoot() + "/src/test/java/org/smcql/plan/operators/codegen/expected/" + testName;
		String observedDir = Utilities.getCodeGenTarget() + "/" + testName;
		assertEquals(true, Utilities.dirsEqual(expectedDir, observedDir));
	}
	*/
	
	protected void testCase(String testName, String sql) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		Logger logger = SystemConfiguration.getInstance().getLogger();
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
			
		QueryCompiler qc = new QueryCompiler(secRoot);
		// TODO: generalize this to write to one file for emp
		qc.writeToDisk();
		
		
		ExecutionStep root = qc.getRoot();
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		
		//compiles code to a couple of cpp files that will be sent to alice & bob for execution
		//String expectedDir = Utilities.getSMCQLRoot() + "/src/test/java/org/smcql/plan/operators/codegen/expected/" + testName;
		//String observedDir = Utilities.getCodeGenTarget() + "/" + testName;
		//assertEquals(true, Utilities.dirsEqual(expectedDir, observedDir));
	
		
	}
	
	private String executeCommand(String command) {

		StringBuffer output = new StringBuffer();

		Process p;
		try {
			p = Runtime.getRuntime().exec(command);
			p.waitFor();
			BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line = "";			
			while ((line = reader.readLine())!= null) {
				output.append(line + "\n");
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		return output.toString();
	}
	
}
