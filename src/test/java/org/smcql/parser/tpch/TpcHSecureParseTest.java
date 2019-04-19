package org.smcql.parser.tpch;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;

import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.sql.SqlExplainFormat;
import org.apache.calcite.sql.SqlExplainLevel;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;

public class TpcHSecureParseTest extends TpcHBaseTest {
	
	  protected void setUp() throws Exception {
		    
		 super.setUp();
			
	  }
	  
	  
	  public void testQuery01() throws Exception {
		     String sql = QUERIES.get(0);
		     String testName = "q" + String.valueOf(1);
		     secureParseTest(testName, sql);
		}

		public void testQuery02() throws Exception {
			 String sql = QUERIES.get(1);
		     String testName = "q" + String.valueOf(2);
		     secureParseTest(testName, sql);
		}

		public void testQuery03() throws Exception {
		     String sql = QUERIES.get(2);
		     String testName = "q" + String.valueOf(3);
		     secureParseTest(testName, sql);
		}

		public void testQuery04() throws Exception {
		     String sql = QUERIES.get(3);
		     String testName = "q" + String.valueOf(4);
		     secureParseTest(testName, sql);
		}

		public void testQuery05() throws Exception {
		     String sql = QUERIES.get(4);
		     String testName = "q" + String.valueOf(5);
		     secureParseTest(testName, sql);
		}

		// want this to be a split aggregate
		// each DO produces a partial sum
		public void testQuery06() throws Exception {
		     String sql = QUERIES.get(5);
		     String testName = "q" + String.valueOf(6);
		     secureParseTest(testName, sql);
		}

		public void testQuery07() throws Exception {
		     String sql = QUERIES.get(6);
		     String testName = "q" + String.valueOf(7);
		     secureParseTest(testName, sql);
		}

		public void testQuery08() throws Exception {
		     String sql = QUERIES.get(7);
		     String testName = "q" + String.valueOf(8);
		     secureParseTest(testName, sql);
		}

		public void testQuery09() throws Exception {
		     String sql = QUERIES.get(8);
		     String testName = "q" + String.valueOf(9);
		     secureParseTest(testName, sql);
		}

		public void testQuery10() throws Exception {
		     String sql = QUERIES.get(9);
		     String testName = "q" + String.valueOf(10);
		     secureParseTest(testName, sql);
		}

		public void testQuery11() throws Exception {
		     String sql = QUERIES.get(10);
		     String testName = "q" + String.valueOf(11);
		     secureParseTest(testName, sql);
		}

		public void testQuery12() throws Exception {
		     String sql = QUERIES.get(11);
		     String testName = "q" + String.valueOf(12);
		     secureParseTest(testName, sql);
		}

		public void testQuery13() throws Exception {
		     String sql = QUERIES.get(12);
		     String testName = "q" + String.valueOf(13);
		     secureParseTest(testName, sql);
		}


	public void testQuery23() throws Exception {
		String sql = QUERIES.get(22);
		String testName = "q" + String.valueOf(13);
		secureParseTest(testName, sql);
	}

	public void testQuery14() throws Exception {
		     String sql = QUERIES.get(13);
		     String testName = "q" + String.valueOf(14);
		     secureParseTest(testName, sql);
		}

		public void testQuery15() throws Exception {
		     String sql = QUERIES.get(14);
		     String testName = "q" + String.valueOf(15);
		     secureParseTest(testName, sql);
		}

		public void testQuery16() throws Exception {
		     String sql = QUERIES.get(15);
		     String testName = "q" + String.valueOf(16);
		     secureParseTest(testName, sql);
		}

		public void testQuery17() throws Exception {
		     String sql = QUERIES.get(16);
		     String testName = "q" + String.valueOf(17);
		     secureParseTest(testName, sql);
		}

		public void testQuery18() throws Exception {
		     String sql = QUERIES.get(17);
		     String testName = "q" + String.valueOf(18);
		     secureParseTest(testName, sql);
		}

		public void testQuery19() throws Exception {
		     String sql = QUERIES.get(18);
		     String testName = "q" + String.valueOf(19);
		     secureParseTest(testName, sql);
		}

		public void testQuery20() throws Exception {
		     String sql = QUERIES.get(19);
		     String testName = "q" + String.valueOf(20);
		     secureParseTest(testName, sql);
		}

		public void testQuery21() throws Exception {
		     String sql = QUERIES.get(20);
		     String testName = "q" + String.valueOf(21);
		     secureParseTest(testName, sql);
		}

		public void testQuery22() throws Exception {
		     String sql = QUERIES.get(21);
		     String testName = "q" + String.valueOf(22);
		     secureParseTest(testName, sql);
		}

		
	  
	  private void secureParseTest(String testName, String sql) throws Exception {
		  // make the test reproducible
		  SystemConfiguration.getInstance().resetCounters();
		  
		  logger.info(testName + " parsing:\n" +sql);
		     
		  SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		  // verifies that the plan resolver identified all attributes and 
		  // matched them to their security policy
		  String plan = RelOptUtil.dumpPlan("", secRoot.getRelRoot().rel, SqlExplainFormat.TEXT, SqlExplainLevel.ALL_ATTRIBUTES);
		  logger.info("Resolved secure tree to:\n " + plan); 
		  
		  // tests if the query compiler knows how to generate this code
		  // parts of this are not written yet for EMP
		  // and have placeholders instead
		  // Placeholders: 
		  //    - conf/smc/operators/emp/sort/keyed.txt
		  
		  QueryCompiler qc = new QueryCompiler(secRoot);

		  ExecutionStep root = qc.getRoot();
		  String testTree = root.printTree();
		  logger.info("Compiled plan:\n" + testTree);
		  
		  
	  }



}
