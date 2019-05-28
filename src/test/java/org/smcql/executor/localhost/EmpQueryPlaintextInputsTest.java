package org.smcql.executor.localhost;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;


public class EmpQueryPlaintextInputsTest extends BaseTest {
  public List<WorkerConfiguration> workers;

  protected void setUp() throws Exception {
	  super.setUp();

    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    workers = new ArrayList<WorkerConfiguration>(cm.getWorkerConfigurations());

  }

  public void testCountIcd9s() throws Exception {

    String query = "SELECT COUNT(DISTINCT major_icd9) FROM diagnoses";
    // to run in plaintext to verify our results
    String testName = "CountIcd9s";


    testCase(testName, query);
  }

  // TODO: Keith please work on getting this going
  // use the examples in here: https://github.com/johesbater/emp-aqp/tree/master/test
  // for guidance
  public void testJoin() throws Exception {
    String testName = "JoinCdiff";
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";

    testCase(testName, query);
  }

  public void testFilterDistinct() throws Exception {
    String testName = "FilterDistinct";
    String query = "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'";
    testCase(testName, query);
  }

  protected QueryTable getExpectedOutput(String testName, String query)
      throws Exception {

	  	String unionedId = ConnectionManager.getInstance().getUnioned();

        SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);

    return SqlQueryExecutor.query(query, outSchema, unionedId);
  }

  public void testScalarAggregate() throws Exception {

    String testName = "testScalarAggregate";

    String query = "SELECT Min(month_id), Max(month_id) FROM diagnoses";
    testCase(testName, query);
  }



  QueryTable getPlainInput(String sql) throws Exception {

    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(sql);
    String workerId = workers.get(0).workerId;

    QueryTable table = SqlQueryExecutor.query(sql, outSchema, workerId);
    return  table;

  }

  protected void testCase(String testName, String sql) throws Exception {
    SystemConfiguration.getInstance().resetCounters();
    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);



    System.out.println("Initial schema: " + secRoot.getPlanRoot().getSchema() );
    QueryCompiler qc = new QueryCompiler(secRoot);

    // Gets all sql inputs that emp code will use - this is the plaintext execution
    // that will generate results where the dummy is not yet separated
    Map<String,String> inputs = qc.getEmpInputs();


    for (Map.Entry<String, String> entry : inputs.entrySet()) {
      String key = entry.getKey();
      String value = entry.getValue();

      // Print the initial function, query, and schema for reference
      System.out.println(key);
      System.out.println(value);
      System.out.println(Utilities.getOutSchemaFromSql(value).toString());

      // run the query against one of the test dbs
      QueryTable wDummy = getPlainInput(value);

      int totalSize = wDummy.toBinaryString().length();

      System.out.println(wDummy.tupleCount());


      // find and remove the dummy tags
      wDummy.extractDummys();
      String dummys = wDummy.getDummyTags();
      System.out.println(wDummy.tupleCount());


      // Print the dummys and the new schema (which should no longer have the bool attached)
      System.out.println(dummys);
      System.out.println(wDummy.getSchema().toString());

      int newSize = dummys.length() + wDummy.toBinaryString().length();


      // making sure new and old size are the same
      assertEquals(totalSize, newSize);



    }


  }
}
