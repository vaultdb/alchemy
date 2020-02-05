package org.vaultdb.util;

import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;

import java.util.ArrayList;

public class Histograms {

  // TODO(@cfgong) Run a histogram query with equal width bins. This requires
  public ArrayList<QueryTable> getHistogram(
      String table_name, ArrayList<String> column_names, int num_bins) throws Exception {

    // TODO(@cfgong) example of how to query using SqlQueryExecutor
    // https://github.com/smcql/smcql/blob/master/src/main/java/org/smcql/plan/execution/slice/statistics/StatisticsCollector.java
    QueryTable t = SqlQueryExecutor.query("SELECT l_orderkey FROM lineitem LIMIT 10");

    return null;
  }
}
