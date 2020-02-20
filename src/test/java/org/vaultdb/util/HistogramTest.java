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
      assertEquals(q1.tupleCount(), numOfBins);
      assertEquals(q2.tupleCount(), numOfBins);
      assertEquals(q1, q2);
    }
  }

  public void testCreateHistogramsSameBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("clean_icd9");
    int numOfBins = 2;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  // This test tests Alice and Bob having different keys
  public void testCreateHistogramsSameBins1() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 9;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  public void testCreateHistogramsMoreBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("clean_icd9");
    int numOfBins = 8;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  public void testCreateHistogramsMoreBins1() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 10;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  public void testCreateHistogramsLessBins() throws Exception {
    // aggregate results, consolidate bins
    // TODO: create more less bins tests: following example below
    // 13 results / 6 bins = 2 r 1 --> 3, 3, 3, 3, 1, 1, 1 --> 2, 2, 2, 2, 2, 2, 3
    // 33 / 7 = 4 r 5 --> 5, 5, 5, 5, 5, 5, 3 --> 4, 4, 5, 5, 5, 5, 5
    // 9 / 6 = 1 r 3 --> 1, 1, 1, 2, 2, 2
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("clean_icd9");
    int numOfBins = 1;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }

  public void testCreateHistogramsLessBins1() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("month_id");
    int numOfBins = 6;

    ArrayList<QueryTable> results = histogram.getHistograms(tableName, columnNames, numOfBins);
    ArrayList<QueryTable> resultsUnioned = histogram.getHistograms(tableName, columnNames, numOfBins, ConnectionManager.getInstance().getUnioned());
    testCorrect(numOfBins, results, resultsUnioned);
  }
}
