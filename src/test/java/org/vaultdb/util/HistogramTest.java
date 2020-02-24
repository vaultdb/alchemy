package org.vaultdb.util;

import java.util.ArrayList;

import org.vaultdb.TpcHBaseTest;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.config.ConnectionManager;


public class HistogramTest extends TpcHBaseTest {

  private void testCorrect(int numOfBins, ArrayList<QueryTable> results, ArrayList<QueryTable> resultsUnioned) {
    for (int i = 0; i < results.size(); i++) {
      QueryTable q1 = results.get(i);
      QueryTable q2 = resultsUnioned.get(i);
      System.out.printf("Query Table %d: %s \n", i, q1);
      assertEquals(numOfBins, q1.tupleCount());
      assertEquals(numOfBins, q2.tupleCount());
      assertEquals(q1, q2);
    }
  }

  // This test tests Alice and Bob having different keys
  // Range divides evenly (integer-wise) into bins
  public void testCreateHistogramsIntegerBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 3;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  // Histogram returns one bin
  public void testCreateHistogramsOneBin() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 1;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  // Range does not divide evenly (integer-wise) into bins
  public void testCreateHistogramsFloatBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 5;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
  // Range does not divide evenly (integer-wise) into bins
  // Some bins will be empty
  public void testCreateHistogramsEmptyBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 10;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
  // Number of bins greater than range
  public void testCreateHistogramsBinCountGreaterThanRange() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 22;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
}
