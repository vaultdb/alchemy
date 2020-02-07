package org.vaultdb.util;

import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class Histograms {

  // TODO(@cfgong) Run a histogram query with equal width bins. This requires
  public ArrayList<QueryTable> getHistogram(
      String tableName, ArrayList<String> columnNames, int numBins) throws Exception {

    // TODO(@cfgong) example of how to query using SqlQueryExecutor
    // https://github.com/smcql/smcql/blob/master/src/main/java/org/smcql/plan/execution/slice/statistics/StatisticsCollector.java
    // Should i get all the columns at once and then count number of occurences?
    /**
     * final String columnString = columnNames.stream().collect(Collectors.joining(", ")); final
     * String query = String.format("SELECT %s FROM %s", columnString, tableName); QueryTable t =
     * SqlQueryExecutor.query(query); for (Tuple tup: t.tuples()){}
     */
    // or should I run through all the columns
    ArrayList<QueryTable> allQueryTables = new ArrayList<>();
    for (String col : columnNames) {
      String query2 =
          String.format(
              "SELECT COUNT(%s) FROM %s GROUP BY %s ORDER BY %s", col, tableName, col, col);
      QueryTable t2 = SqlQueryExecutor.query(query2);
      final int tupleCount = t2.tupleCount();
      if (tupleCount > numBins) {
        // aggregate results, consolidate bins
        // do some math to determine how many bins
        // 13 results / 6 bins = 2 r 1 --> 3, 3, 3, 3, 1, 1, 1 --> 2, 2, 2, 2, 2, 2, 3
        // 33 / 7 = 4 r 5 --> 5, 5, 5, 5, 5, 5, 3 --> 4, 4, 5, 5, 5, 5, 5
        // 9 / 6 = 1 r 3 --> 1, 1, 1, 2, 2, 2
        final List<Tuple> tuples = t2.tuples();
        final int resultsPerBinMin = tupleCount / numBins;
        int binsToModifyMin = numBins - tupleCount % numBins;
        final int resultsPerBinMax = (int) Math.ceil(tupleCount / numBins * 1.0);
        int binsToModifyMax = tupleCount % numBins;
        int index = 0;
        if (resultsPerBinMin > 1) {
          while (binsToModifyMin > 0) {
            for (int i = index; i < resultsPerBinMin; i++) {
              // somehow sum the tuples here
            }
            index += resultsPerBinMin;
            binsToModifyMin--;
          }
        }
        while (binsToModifyMax > 0) {
          for (int i = index; i < resultsPerBinMax; i++) {
            // somehow sum the tuples here
          }
          index += resultsPerBinMax;
          binsToModifyMin--;
        }
        allQueryTables.add(t2);

      } else if (tupleCount < numBins) {
        // more bins needed, add 0 padding
        int numBinsToAdd = tupleCount - numBins;
        // TODO: how to make tuples?
        for (int i = 0; i < numBinsToAdd; i++) {
          // add 0 tuples here
        }
      } else if (tupleCount == numBins) {
        allQueryTables.add(t2);
      }
    }

    return null;
  }
}
