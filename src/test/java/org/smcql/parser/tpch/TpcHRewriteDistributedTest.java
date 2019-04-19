package org.smcql.parser.tpch;
import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;

public class TpcHRewriteDistributedTest  extends TpcHBaseTest {

		
		
		
		  protected void setUp() throws Exception {
			    super.setUp();
		  }
		  
		  public void testQuery01() throws Exception {
			     String sql = QUERIES.get(0);
			     String testName = "q" + String.valueOf(1);
			     testCase(testName, sql);
			}

			public void testQuery02() throws Exception {
				 String sql = QUERIES.get(1);
			     String testName = "q" + String.valueOf(2);
			     testCase(testName, sql);
			}

			public void testQuery03() throws Exception {
			     String sql = QUERIES.get(2);
			     String testName = "q" + String.valueOf(3);
			     testCase(testName, sql);
			}

			public void testQuery04() throws Exception {
			     String sql = QUERIES.get(3);
			     String testName = "q" + String.valueOf(4);
			     testCase(testName, sql);
			}

			public void testQuery05() throws Exception {
			     String sql = QUERIES.get(4);
			     String testName = "q" + String.valueOf(5);
			     testCase(testName, sql);
				// TODO - Improve efficiency.  This works but takes a long time

			}

			public void testQuery06() throws Exception {
			     String sql = QUERIES.get(5);
			     String testName = "q" + String.valueOf(6);
			     testCase(testName, sql);
			}

			public void testQuery07() throws Exception {
			     String sql = QUERIES.get(6);
			     String testName = "q" + String.valueOf(7);
			     testCase(testName, sql);
			}

			public void testQuery08() throws Exception {
			     String sql = QUERIES.get(7);
			     String testName = "q" + String.valueOf(8);
			     testCase(testName, sql);
			}

			public void testQuery09() throws Exception {
			     String sql = QUERIES.get(8);
			     String testName = "q" + String.valueOf(9);
			     testCase(testName, sql);
			}

			public void testQuery10() throws Exception {
			     String sql = QUERIES.get(9);
			     String testName = "q" + String.valueOf(10);
			     testCase(testName, sql);
			}

			public void testQuery11() throws Exception {
			     String sql = QUERIES.get(10);
			     String testName = "q" + String.valueOf(11);
			     testCase(testName, sql);
			}

			public void testQuery12() throws Exception {
			     String sql = QUERIES.get(11);
			     String testName = "q" + String.valueOf(12);
			     testCase(testName, sql);
			}

			public void testQuery13() throws Exception {
			     String sql = QUERIES.get(12);
			     String testName = "q" + String.valueOf(13);
			     testCase(testName, sql);
			}

			public void testQuery14() throws Exception {
			     String sql = QUERIES.get(13);
			     String testName = "q" + String.valueOf(14);
			     testCase(testName, sql);
			}

			public void testQuery15() throws Exception {
			     String sql = QUERIES.get(14);
			     String testName = "q" + String.valueOf(15);
			     testCase(testName, sql);
			}

			public void testQuery16() throws Exception {
			     String sql = QUERIES.get(15);
			     String testName = "q" + String.valueOf(16);
			     testCase(testName, sql);
			}

			public void testQuery17() throws Exception {
			     String sql = QUERIES.get(16);
			     String testName = "q" + String.valueOf(17);
			     testCase(testName, sql);
			}

			public void testQuery18() throws Exception {
			     String sql = QUERIES.get(17);
			     String testName = "q" + String.valueOf(18);
			     testCase(testName, sql);
			}

			public void testQuery19() throws Exception {
			     String sql = QUERIES.get(18);
			     String testName = "q" + String.valueOf(19);
			     testCase(testName, sql);
			}

			public void testQuery20() throws Exception {
			     String sql = QUERIES.get(19);
			     String testName = "q" + String.valueOf(20);
			     testCase(testName, sql);
			}

			public void testQuery21() throws Exception {
			     String sql = QUERIES.get(20);
			     String testName = "q" + String.valueOf(21);
			     testCase(testName, sql);
			}

			public void testQuery22() throws Exception {
			     String sql = QUERIES.get(21);
			     String testName = "q" + String.valueOf(22);
			     testCase(testName, sql);
			}


			
		  
		  protected void testCase(String testName, String sql) throws Exception {

		    SystemConfiguration.getInstance().resetCounters();
		    logger.info(testName + " parsing:\n" + sql);
		    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);

			String distributedSql = SqlGenerator.getDistributedSql(secRoot, SystemConfiguration.DIALECT);
			logger.info("Produced distributed query:\n" + distributedSql);


		    SecureRelRecordType outSchema = secRoot.getPlanRoot().getSchema();
		    QueryTable expectedOutput = super.getExpectedDistributedOutput(sql, outSchema);
			QueryTable observedOutput = super.getExpectedOutput(distributedSql, outSchema);

			
			assertEquals(expectedOutput.tupleCount(), observedOutput.tupleCount());
			assertEquals(expectedOutput, observedOutput);
		 
		    
		  }
		  
		  
			 

}
