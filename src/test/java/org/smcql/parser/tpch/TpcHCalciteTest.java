package org.smcql.parser.tpch;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.rel2sql.RelToSqlConverter;
import org.apache.calcite.sql.SqlExplainFormat;
import org.apache.calcite.sql.SqlExplainLevel;
import org.apache.calcite.sql.SqlNode;


public class TpcHCalciteTest extends TpcHBaseTest {

	Map<String, ArrayList<RelNode> > operatorHistogram;
	Map<String, Integer> globalOperatorCounts;
	
	
	
	  protected void setUp() throws Exception {
		    super.setUp();
		    globalOperatorCounts = new HashMap<String, Integer>();
		    operatorHistogram = new HashMap<String, ArrayList<RelNode> >();			
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
	    

		// Do we still have this problem? Looks like it is solved...

        // for Q13, why is stack padded with a bunch of nulls?
        // failing to add last table scan to its queue before first pop in visit child?
        // no, this goes through, but it first fails on the join
        // let's try isolating the join
        
	    // getting buried for now
	    //logger.info("For test " + testName + " running:\n" + sql);


	    RelRoot root = parser.convertSqlToRelMinFields(sql);
	    
	    
	    String plan = RelOptUtil.dumpPlan("", root.rel, SqlExplainFormat.TEXT, SqlExplainLevel.ALL_ATTRIBUTES);

	    logger.info("Parsed plan for " + testName + ":\n" + plan);
	    
	    String query = SqlGenerator.getSql(root, SystemConfiguration.DIALECT);
	    logger.info("Preparing to run query: " + query);
	    SqlQueryExecutor.queryNoOutput(query, "alice");



	    // Reverse our own plan using our SqlGenerator
	    String sqlOut = SqlGenerator.getSql(root.rel,SystemConfiguration.DIALECT);
	    System.out.println("Sql deparsed: " + sqlOut);



	    // Build and log tree histogram
	    buildOperatorHistogram(root.rel);

	    String histogram = "Operator distribution for " + testName + ":\n";
	    
	    for(Entry<String, ArrayList<RelNode> > opType : operatorHistogram.entrySet()) {
	    	histogram += "     " + opType.getKey() + ": " + opType.getValue().size() + "\n";
	    }
	    
	    logger.info(histogram);

	    if(testName.equals("q1")) 
	    	initializeOperatorCounts(operatorHistogram);
	    else
	    	mergeOperatorCounts(operatorHistogram);

	    
	    logger.info("Global counts histogram:\n" + globalOperatorCounts.toString().replace(' ', '\n'));

	  }
	  
	  
		 
	 private void initializeOperatorCounts(Map<String, ArrayList<RelNode>> localCounts) throws IOException {
		  for(Entry<String, ArrayList<RelNode> > entry : localCounts.entrySet()) {
			  String opType = entry.getKey();
			  Integer cnt = entry.getValue().size();
			  globalOperatorCounts.put(opType, cnt);
		  }
	
		  // pass it on to the next junit test
		  System.setProperty("global.operator.count", serialize((Serializable) globalOperatorCounts));
	 }
	 
	 private void mergeOperatorCounts(Map<String, ArrayList<RelNode>> localCounts) throws ClassNotFoundException, IOException {
		 

		 String serializedMap = System.getProperty("global.operator.count");
		 globalOperatorCounts = (Map<String, Integer>) deserialize(serializedMap);
		 
		  for(Entry<String, ArrayList<RelNode> > entry : localCounts.entrySet()) {
			  String opType = entry.getKey();
			  Integer cnt = entry.getValue().size();
			  if(globalOperatorCounts.containsKey(opType)) {
				  cnt +=  globalOperatorCounts.get(opType);
			  }
			  globalOperatorCounts.put(opType, cnt);
		  }
		  
		  // serializing to persist state between unit tests
		  System.setProperty("global.operator.count", serialize((Serializable) globalOperatorCounts));
	  }
	  

	  /// helper methods ///
	 // keep track of relnodes
	  private void buildOperatorHistogram(RelNode rel ){
		  String type = rel.getRelTypeName();
		  if(operatorHistogram.containsKey(type)) {
			  ArrayList<RelNode> entries = operatorHistogram.get(type);
			  entries.add(rel);
			  operatorHistogram.put(type, entries);
  
		  }
		  else {
			  ArrayList<RelNode> entries = new ArrayList<RelNode>();
			  entries.add(rel);
			  operatorHistogram.put(type, entries);			  
		  }
		  
		  
		  for(RelNode child : rel.getInputs()) {
			  buildOperatorHistogram(child);
		  }
		  
	  }
	  
	  
	  
	  
	  private static String serialize(Serializable o) throws IOException {
		    ByteArrayOutputStream baos = new ByteArrayOutputStream();
		    ObjectOutputStream oos = new ObjectOutputStream(baos);
		    oos.writeObject(o);
		    oos.close();
		    return Base64.getEncoder().encodeToString(baos.toByteArray());
		}
	
	  
	  private static Object deserialize(String s) throws IOException,
      ClassNotFoundException {
		  byte[] data = Base64.getDecoder().decode(s);
		  ObjectInputStream ois = new ObjectInputStream(
          new ByteArrayInputStream(data));
		  Object o = ois.readObject();
		  ois.close();
		  return o;
}

}
