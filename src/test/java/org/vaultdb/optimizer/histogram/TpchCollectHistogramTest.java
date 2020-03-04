package org.vaultdb.optimizer.histogram;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.vaultdb.BaseTest;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.sql.SqlGenerator;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.Field;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Histograms;
import org.vaultdb.util.Utilities;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class TpchCollectHistogramTest extends TpcHBaseTest {
  /**
   * List of relations:
   * customer | lineitem | nation | orders | part | partsupp | region | supplier
   */
  /**
   * table: customer
   * primary_key: c_custkey (integer)
   */
  private void testCorrect(int numOfBins, ArrayList<QueryTable> results, ArrayList<QueryTable> resultsUnioned) {
    for (int i = 0; i < results.size(); i++) {
      QueryTable q1 = results.get(i);
      QueryTable q2 = resultsUnioned.get(i);
//      System.out.printf("Query Table %d: %s \n", i, q1);
      assertEquals(numOfBins, q1.tupleCount());
      assertEquals(numOfBins, q2.tupleCount());
      assertEquals(q1, q2);
    }
  }

  public void testCollectDiagnoses() throws Exception {

    final FrameworkConfig calciteConfig = config.getCalciteConfiguration();
    RelBuilder builder = RelBuilder.create(calciteConfig);
    RelNode node =
        builder
            .scan("customer")
            .aggregate(builder.groupKey("c_custkey"), builder.count(false, "cnt"))
            .build();

    String query = SqlGenerator.getSql(node, config.DIALECT);
    System.out.println("Query: \n" + query);

    String aId = ConnectionManager.getInstance().getAlice();
    String bId = ConnectionManager.getInstance().getBob();

    SecureRelRecordType dstSchema = Utilities.getOutSchemaFromSql(query);
    QueryTable aTable = SqlQueryExecutor.query(query, dstSchema, aId);
    QueryTable bTable = SqlQueryExecutor.query(query, dstSchema, bId);

    System.out.println("A table: " + aTable);
    System.out.println("B table: " + bTable);
  }
  /**
   * List of relations:
   * customer | lineitem | nation | orders | part | partsupp | region | supplier
   */
  /**
   * table: customer
   * primary_key: c_custkey (integer)
   * foreign_key: c_nationkey (integer)
   * referenced_by: orders.o_custkey (integer)
   */
  public void testCustomerHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "customer";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("c_nationkey");
    int numOfBins = 19;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
  /**
   * table: lineitem
   * primary_key: (l_orderkey, l_linenumber) (integer, integer)
   * foreign_key: l_orderkey (integer)
   */
  public void testLineitemHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "lineitem";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("l_orderkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
  /**
   * table: nation
   * primary_key: n_nationkey (integer)
   * foreign_key:  n_regionkey (integer)
   * referenced_by: customer.c_nationkey (integer)
   * referenced_by: supplier.s_nationkey (integer)
   */
  public void testNationHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "nation";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("n_regionkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  /**
   * table: orders
   * primary_key: o_orderkey (integer)
   * foreign_key:  o_custkey (integer)
   * referenced_by: lineitem.l_orderkey (integer)
   */
  public void testOrdersHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "orders";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("o_custkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  /**
   * table: part
   * primary_key: p_partkey (integer)
   * referenced_by: partsupp.ps_partkey (integer)
   */
  public void testPartHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "part";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("p_partkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  /**
   * table: partsupp
   * primary_key: ps_partkey (integer)
   * foreign_key: ps_partkey (integer)
   * foreign_key: ps_suppkey (integer)
   */
  public void testPartsuppHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "partsupp";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("ps_suppkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  /**
   * table: region
   * primary_key: r_regionkey (integer)
   * foreign_key: r_regionkey (integer)
   */
  public void testRegionHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "region";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("r_regionkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  /**
   * table: supplier
   * primary_key: s_suppkey (integer)
   * foreign_key: s_nationkey (integer)
   * referenced_by: partsupp.ps_suppkey (integer)
   */
  public void testSupplierHistogram() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "supplier";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("s_nationkey");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

}
