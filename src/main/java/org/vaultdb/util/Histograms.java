package org.vaultdb.util;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.sql.type.SqlTypeName;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.vaultdb.codegen.sql.SqlGenerator;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.CharField;
import org.vaultdb.db.data.field.Field;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.StringJoiner;

public class Histograms {

  protected SystemConfiguration config;
  protected FrameworkConfig calciteConfig;

  // Constants of the index of fields in count(*) queries
  public static final int GROUPED_BY_FIELD_IDX = 0; // grouped by key (bin) field index
  public static final int COUNT_FIELD_IDX = 1; // count field index

  public Histograms() throws Exception {
    System.setProperty("vaultdb.setup", Utilities.getVaultDBRoot() + "/conf/setup.global");
    SystemConfiguration.resetConfiguration();
    SystemCatalog.resetInstance();
    ConnectionManager.reset();

    config = SystemConfiguration.getInstance();
    calciteConfig = config.getCalciteConfiguration();
  }

  public ArrayList<QueryTable> getHistograms(
      String tableName, ArrayList<String> columnNames, int numBins) throws Exception {
    List<String> workers = ConnectionManager.getInstance().getAliceAndBob();
    ArrayList<QueryTable> finalHistogramList = new ArrayList<>();
    for (String col: columnNames){
      ArrayList<QueryTable> histogramListByWorkers = new ArrayList<>();
      for (String workerId: workers){
        // get query table for each worker
        QueryTable qt = getQueryTable(tableName, workerId, col);
        System.out.println(workerId + qt);
        histogramListByWorkers.add(qt);
      }
      HashMap<Field, Tuple> fieldTupleHashMap = new HashMap<>();
      // combine query tables
      for (QueryTable queryTable: histogramListByWorkers){
        for (Tuple tuple : queryTable.tuples()) {
          if (fieldTupleHashMap.containsKey(tuple.getField(GROUPED_BY_FIELD_IDX))) {
            Tuple aggregatedTuple = getAggregatedTuple(fieldTupleHashMap, tuple);
            fieldTupleHashMap.put(aggregatedTuple.getField(GROUPED_BY_FIELD_IDX), aggregatedTuple);
          } else {
            fieldTupleHashMap.put(tuple.getField(GROUPED_BY_FIELD_IDX), tuple);
          }
        }
      }
      List<Tuple> tupleList = new ArrayList<>(fieldTupleHashMap.values());
      // resize into a proper histogram
      QueryTable summedHistogram = new QueryTable(tupleList);
      finalHistogramList.add(resizeQueryTableToHistogram(numBins, summedHistogram));
    }
    return finalHistogramList;
  }

  private Tuple getAggregatedTuple(HashMap<Field, Tuple> fieldTupleHashMap, Tuple tuple) {
    Tuple aggregatedTuple = new Tuple();
    aggregatedTuple.addField(tuple.getField(GROUPED_BY_FIELD_IDX));

    IntField aCount =
            (IntField)
                    fieldTupleHashMap.get(tuple.getField(GROUPED_BY_FIELD_IDX)).getField(COUNT_FIELD_IDX);
    IntField bCount = (IntField) tuple.getField(COUNT_FIELD_IDX);

    IntField aggregatedCount = new IntField(bCount.getAttribute(), bCount.getSqlTypeName());
    aggregatedCount.setValue(aCount.value + bCount.value);
    aggregatedTuple.addField(aggregatedCount);
    return aggregatedTuple;
  }

  private QueryTable getQueryTable(String tableName, String workerId, String column) throws Exception {
    RelBuilder builder = RelBuilder.create(calciteConfig);
    RelNode node =
            builder
                    .scan(tableName)
                    .aggregate(builder.groupKey(column),
                            builder.count(false, "cnt"))
                    .sort(0)
                    .build();

    String query = SqlGenerator.getSql(node, config.DIALECT);
    System.out.println("Query: \n" + query);
    QueryTable resultTable = SqlQueryExecutor.query(query, workerId);
    return resultTable;
  }

  public ArrayList<QueryTable> getHistograms(
          String tableName, ArrayList<String> columnNames, int numBins, String workerId) throws Exception {
    ArrayList<QueryTable> allQueryTables = new ArrayList<>();
    for (String col : columnNames) {
      allQueryTables.add(getHistogram(tableName, numBins, workerId, col));
    }
    return allQueryTables;
  }

  public QueryTable getHistogram(String tableName, int numBins, String workerId, String column) throws Exception {
    QueryTable resultTable = getQueryTable(tableName, workerId, column);
    System.out.println(resultTable);
    return resizeQueryTableToHistogram(numBins, resultTable);
  }

  private QueryTable resizeQueryTableToHistogram(int numBins, QueryTable resultTable) throws Exception {
    if (numBins <= 0){
      throw new Exception("Number of bins in histogram must be greater than 0");
    }
    final int tupleCount = resultTable.tupleCount();
    final List<Tuple> tuples = resultTable.tuples();
    final List<Tuple> newTuples = new ArrayList<>();
    // get range of keys to decide uniform bin size
    final long maxKey = ((IntField) tuples.get(tupleCount - 1).getField(GROUPED_BY_FIELD_IDX)).getValue();
    final long minKey = ((IntField) tuples.get(0).getField(GROUPED_BY_FIELD_IDX)).getValue();
    final float range = maxKey - minKey;
    final float sizeOfBin = range / numBins;

    IntField currTupleField  = (IntField) tuples.get(0).getField(GROUPED_BY_FIELD_IDX);
    int index = 0;
    float maxInBin = sizeOfBin + minKey;
    // keep aggregating tuples until we run out of tuples
    while (index < tupleCount){
      StringJoiner charFieldStringJoiner = new StringJoiner(", ", "[", "]");

      CharField groupField = new CharField(currTupleField.getAttribute(), SqlTypeName.VARCHAR);
      IntField countField = new IntField(currTupleField.getAttribute(), SqlTypeName.INTEGER);
      // keep adding until we run out of tuples, or grouped by key value is too large for current bin
      while (index < tupleCount &&
              ((IntField)tuples.get(index).getField(GROUPED_BY_FIELD_IDX)).getValue() <= maxInBin){
        IntField oldGroupByField = (IntField) tuples.get(index).getField(GROUPED_BY_FIELD_IDX);
        charFieldStringJoiner.add(String.valueOf(oldGroupByField.value));
        IntField count = (IntField) tuples.get(index).getField(COUNT_FIELD_IDX);
        countField.addValue(count.value);
        index++;
      }
      // create new tuple
      Tuple aggregatedTuple = new Tuple();
      String charFieldString = charFieldStringJoiner.toString();
      groupField.setValue(charFieldString);
      aggregatedTuple.addField(groupField);
      aggregatedTuple.addField(countField);
      newTuples.add(aggregatedTuple);
      // move onto the next bin
      maxInBin += sizeOfBin;
    }

    return new QueryTable(newTuples);
  }
}
