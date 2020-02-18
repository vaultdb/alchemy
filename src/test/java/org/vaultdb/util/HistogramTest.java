package org.vaultdb.util;

import java.util.ArrayList;
import java.util.List;

import org.vaultdb.BaseTest;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.util.Histograms;


public class HistogramTest extends TpcHBaseTest {

  public void testCreateHistogramsSameBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("clean_icd9");
    int numOfBins = 2;

    ArrayList<QueryTable> results = histogram.getHistogram(tableName, columnNames, numOfBins);
    for (int i = 0; i < results.size(); i++){
      QueryTable q = results.get(i);
      System.out.printf("Query Table %d: %s \n", i, q);
      assertEquals(q.tupleCount(), numOfBins);
    }
  }

  public void testCreateHistogramsMoreBins() throws Exception {
    Histograms histogram = new Histograms();

    String tableName = "diagnoses";
    ArrayList<String> columnNames = new ArrayList<>();
    columnNames.add("clean_icd9");
    int numOfBins = 8;

    ArrayList<QueryTable> results = histogram.getHistogram(tableName, columnNames, numOfBins);
    for (int i = 0; i < results.size(); i++){
      QueryTable q = results.get(i);
      System.out.printf("Query Table %d: %s \n", i, q);
      assertEquals(q.tupleCount(), numOfBins);
    }
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

    ArrayList<QueryTable> results = histogram.getHistogram(tableName, columnNames, numOfBins);
    for (int i = 0; i < results.size(); i++){
      QueryTable q = results.get(i);
      System.out.printf("Query Table %d: %s \n", i, q);
      assertEquals(q.tupleCount(), numOfBins);
    }
  }
}
