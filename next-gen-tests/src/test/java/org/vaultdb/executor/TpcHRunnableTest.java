package org.vaultdb.executor;

import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

public class TpcHRunnableTest extends TpcHBaseTest {

	
	  protected void setUp() throws Exception {
		    
		 super.setUp();
			
	  }
	  
	  
	  public void testQuery01() throws Exception {
		     String sql = QUERIES.get(0);
		     String testName = "q" + String.valueOf(1);
		     runQuery(testName, sql);
		}

		public void testQuery02() throws Exception {
			 String sql = QUERIES.get(1);
		     String testName = "q" + String.valueOf(2);
		     runQuery(testName, sql);
		}

		public void testQuery03() throws Exception {
		     String sql = QUERIES.get(2);
		     String testName = "q" + String.valueOf(3);
		     runQuery(testName, sql);
		}

		public void testQuery04() throws Exception {
		     String sql = QUERIES.get(3);
		     String testName = "q" + String.valueOf(4);
		     runQuery(testName, sql);
		}

		public void testQuery05() throws Exception {
		     String sql = QUERIES.get(4);
		     String testName = "q" + String.valueOf(5);
		     runQuery(testName, sql);
		}

		// want this to be a split aggregate
		// each DO produces a partial sum
		public void testQuery06() throws Exception {
		     String sql = QUERIES.get(5);
		     String testName = "q" + String.valueOf(6);
		     runQuery(testName, sql);
		}

		public void testQuery07() throws Exception {
		     String sql = QUERIES.get(6);
		     String testName = "q" + String.valueOf(7);
		     runQuery(testName, sql);
		}

		public void testQuery08() throws Exception {
		     String sql = QUERIES.get(7);
		     String testName = "q" + String.valueOf(8);
		     runQuery(testName, sql);
		}

		public void testQuery09() throws Exception {
		     String sql = QUERIES.get(8);
		     String testName = "q" + String.valueOf(9);
		     runQuery(testName, sql);
		}

		public void testQuery10() throws Exception {
		     String sql = QUERIES.get(9);
		     String testName = "q" + String.valueOf(10);
		     runQuery(testName, sql);
		}

		public void testQuery11() throws Exception {
		     String sql = QUERIES.get(10);
		     String testName = "q" + String.valueOf(11);
		     runQuery(testName, sql);
		}

		public void testQuery12() throws Exception {
		     String sql = QUERIES.get(11);
		     String testName = "q" + String.valueOf(12);
		     runQuery(testName, sql);
		}

		public void testQuery13() throws Exception {
		     String sql = QUERIES.get(12);
		     String testName = "q" + String.valueOf(13);
		     runQuery(testName, sql);
		}

		public void testQuery14() throws Exception {
		     String sql = QUERIES.get(13);
		     String testName = "q" + String.valueOf(14);
		     runQuery(testName, sql);
		}

		public void testQuery15() throws Exception {
		     String sql = QUERIES.get(14);
		     String testName = "q" + String.valueOf(15);
		     runQuery(testName, sql);
		}

		public void testQuery16() throws Exception {
		     String sql = QUERIES.get(15);
		     String testName = "q" + String.valueOf(16);
		     runQuery(testName, sql);
		}

		public void testQuery17() throws Exception {
		     String sql = QUERIES.get(16);
		     String testName = "q" + String.valueOf(17);
		     runQuery(testName, sql);
		}

		public void testQuery18() throws Exception {
		     String sql = QUERIES.get(17);
		     String testName = "q" + String.valueOf(18);
		     runQuery(testName, sql);
		}

		public void testQuery19() throws Exception {
		     String sql = QUERIES.get(18);
		     String testName = "q" + String.valueOf(19);
		     runQuery(testName, sql);
		}

		public void testQuery20() throws Exception {
		     String sql = QUERIES.get(19);
		     String testName = "q" + String.valueOf(20);
		     runQuery(testName, sql);
		}

		public void testQuery21() throws Exception {
		     String sql = QUERIES.get(20);
		     String testName = "q" + String.valueOf(21);
		     runQuery(testName, sql);
		}

		public void testQuery22() throws Exception {
		     String sql = QUERIES.get(21);
		     String testName = "q" + String.valueOf(22);
		     runQuery(testName, sql);
		}

		
	
	protected void runQuery(String testName, String sql) throws Exception {
	    SystemConfiguration.getInstance().resetCounters();
	    
	    logger.info("TpcHRunnableTest::" + testName + " is disabled pending code generator development.");
	    
	    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);

	    logger.info("Initial schema: " + secRoot.getPlanRoot().getSchema() );
	    QueryCompiler qc = new QueryCompiler(secRoot);
	    qc.writeOutEmpFile();
	    String empTarget = Utilities.getCodeGenTarget() + "/" + testName + ".h";
	    String jniTarget = Utilities.getCodeGenTarget() + "/" + testName + ".java";
			
	    assertTrue(FileUtilities.fileExists(empTarget));
	    assertTrue(FileUtilities.fileExists(jniTarget));
	    
	    EmpExecutor exec = new EmpExecutor(qc);
	    exec.runForTesting(); 
		  
	}
}
