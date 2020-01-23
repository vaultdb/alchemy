package org.vaultdb.executor.local;

import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.EmpExecutor;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;


public class TpcHQueryExecutionTest extends TpcHBaseTest {
	  protected void setUp() throws Exception {
		 super.setUp();
			
	  }
	  
	  
	  public void testQuery01() throws Exception {
		     runQuery(1);
		}

	  /*
		public void testQuery02() throws Exception {
			runQuery(2);
		}

		public void testQuery03() throws Exception {
			 runQuery(3);
		}

		public void testQuery04() throws Exception {
			runQuery(4);
		}

		public void testQuery05() throws Exception {
			runQuery(5);
		}

		// want this to be a split aggregate
		// each DO produces a partial sum
		public void testQuery06() throws Exception {
			runQuery(6);
		}

		public void testQuery07() throws Exception {
			runQuery(7);
		}

		public void testQuery08() throws Exception {
			runQuery(8);
		}

		public void testQuery09() throws Exception {
			runQuery(9);
		}

		public void testQuery10() throws Exception {
			runQuery(10);
		}

		public void testQuery11() throws Exception {
		     runQuery(11);
		}

		public void testQuery12() throws Exception {
		     runQuery(12);
		}

		public void testQuery13() throws Exception {
		     runQuery(13);
		}

		public void testQuery14() throws Exception {
		     runQuery(14);
		}

		public void testQuery15() throws Exception {
		     runQuery(15);
		}

		public void testQuery16() throws Exception {
		     runQuery(16);
		}

		public void testQuery17() throws Exception {
		     runQuery(17);
		}

		public void testQuery18() throws Exception {
		     runQuery(18);
		}

		public void testQuery19() throws Exception {
		     runQuery(19);
		}

		public void testQuery20() throws Exception {
		     runQuery(20);
		}

		public void testQuery21() throws Exception {
		     runQuery(21);
		}

		public void testQuery22() throws Exception {
		     runQuery(22);
		}
*/
		
	
	protected void runQuery(int queryID) throws Exception {
		String testName = "q" + String.valueOf(queryID);
		String sql = readSQL(queryID);
		
	    SystemConfiguration.getInstance().resetCounters();
	    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	    SecureRelRecordType outSchema = secRoot.getPlanRoot().getSchema();
	    QueryTable expectedOutput = getExpectedOutput(sql, outSchema);

	    logger.info("Generating code for tree:\n" + secRoot);
	    
	    QueryCompiler qc = new QueryCompiler(secRoot);
	    qc.writeOutEmpFile();
	    String empTarget = Utilities.getCodeGenTarget() + "/" + testName + ".h";
	    String jniTarget = Utilities.getCodeGenTarget() + "/" + testName + ".java";
			
	    assertTrue(FileUtilities.fileExists(empTarget));
	    assertTrue(FileUtilities.fileExists(jniTarget));
	    
	    EmpExecutor exec = new EmpExecutor(qc);
	    exec.run();
	    
	    QueryTable observedOutput = exec.getOutput();
	    assertEquals(expectedOutput.tupleCount(), observedOutput.tupleCount());
	    assertEquals(expectedOutput, observedOutput);
		  
	}
	
	
}