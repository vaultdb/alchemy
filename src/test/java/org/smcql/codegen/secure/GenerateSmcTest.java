package org.smcql.codegen.secure;

import org.smcql.BaseTest;
import org.smcql.codegen.CodeCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;

public class GenerateSmcTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}
	
	public void testComorbidity() throws Exception {
		testCase("comorbidity");
	}
	

	public void testCDiff() throws Exception {
		testCase("cdiff");
	}

	public void testAspirinCount() throws Exception {
		testCase("aspirin-count");
	}
	
	
	protected void testCase(String testName) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		//compiles code to a couple of cpp files that will be sent to alice & bob for execution
		CodeCompiler cc = new CodeCompiler(secRoot);
		cc.compile(Utilities.getCodeGenTarget() + "/" + testName);
		System.out.println("Query: " + testName.toUpperCase());
		System.out.println("\nTree:\n" + cc.getTree());
		System.out.println("\nSource SQL:\n" + cc.getSourceSQL());
		//System.out.println("Source SQL:");
		// cc.printSourceSQL();
		//System.out.println("\nEMP Code:");
		//cc.printEmpCode();
		/*
		QueryCompiler qc = new QueryCompiler(secRoot);
		qc.writeToDisk();
		
		ExecutionStep root = qc.getRoot();
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		
		testName = testName.replace("-", "_");
		String expectedDir = Utilities.getSMCQLRoot() + "/src/test/java/org/smcql/plan/operators/codegen/expected/" + testName;
		String observedDir = Utilities.getCodeGenTarget() + "/" + testName;
		assertEquals(true, Utilities.dirsEqual(expectedDir, observedDir));*/
	}
	
}
