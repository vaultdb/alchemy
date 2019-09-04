package org.vaultdb.compiler.emp;


import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.sql.SqlExplainFormat;
import org.apache.calcite.sql.SqlExplainLevel;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.plan.SecureRelRoot;

public class QueryCompilerTpcHTest extends TpcHBaseTest {
	
	  protected void setUp() throws Exception {
		    
		 super.setUp();
			
	  }
	  
	  
	  public void testQuery01() throws Exception {
		     secureParseTest(1);
		}

		public void testQuery02() throws Exception {
		     secureParseTest(2);
		}

		public void testQuery03() throws Exception {
		     secureParseTest(3);
		}

		public void testQuery04() throws Exception {
		     secureParseTest(4);
		}

		public void testQuery05() throws Exception {
		     secureParseTest(5);
		}

		// want this to be a split aggregate
		// each DO produces a partial sum
		public void testQuery06() throws Exception {
		     secureParseTest(6);
		}

		// TODO: extract year by pushing down this expression to outside of MPC
		public void testQuery07() throws Exception {
		     secureParseTest(7);
		}

		public void testQuery08() throws Exception {
		     secureParseTest(8);
		}

		public void testQuery09() throws Exception {
		     secureParseTest(9);
		}

		public void testQuery10() throws Exception {
		     secureParseTest(10);
		}

		public void testQuery11() throws Exception {
		     secureParseTest(11);
		}

		public void testQuery12() throws Exception {
		     secureParseTest(12);
		}

		public void testQuery13() throws Exception {
		     secureParseTest(13);
		}




	public void testQuery14() throws Exception {
	     secureParseTest(14);
		}

		public void testQuery15() throws Exception {
		     secureParseTest(15);
		}

		public void testQuery16() throws Exception {
		     secureParseTest(16);
		}

		public void testQuery17() throws Exception {
		     secureParseTest(17);
		}

		public void testQuery18() throws Exception {
		     secureParseTest(18);
		}

		public void testQuery19() throws Exception {
		     secureParseTest(19);
		}

		public void testQuery20() throws Exception {
		     secureParseTest(20);
		}

		public void testQuery21() throws Exception {
		     secureParseTest(21);
		}

		public void testQuery22() throws Exception {
		     secureParseTest(22);
		}

		
	  
	  private void secureParseTest(int testNo) throws Exception {
		  // make the test reproducible
		  config.resetCounters();
		  
		  String sql = super.readSQL(testNo);
		  String testName = "q" + testNo;
		  
		  logger.info(testName + " parsing:\n" +sql);
		     
		  SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		  // verifies that the plan resolver identified all attributes and 
		  // matched them to their security policy
		  String plan = RelOptUtil.dumpPlan("", secRoot.getRelRoot().rel, SqlExplainFormat.TEXT, SqlExplainLevel.NON_COST_ATTRIBUTES);
		  logger.info("Resolved secure tree to:\n " + plan); 
		  
		  // tests if the query compiler knows how to generate this code
		  // parts of this are not written yet for EMP
		  // and have placeholders instead
		  // Placeholders: 
		  //    - conf/smc/operators/emp/sort/keyed.txt
		  
		  QueryCompiler qc = new QueryCompiler(secRoot);

		  ExecutionStep root = qc.getRoot();
		  
		  qc.writeOutEmpFile();

		  String testTree = root.printTree();
		  logger.info("Compiled plan:\n" + testTree);
		  
		  
	  }



}
