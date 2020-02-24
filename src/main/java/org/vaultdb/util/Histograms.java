package org.vaultdb.util;

import org.apache.calcite.rel.RelNode;
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
  public static final int CHAR_FIELD_IDX = 0; // grouped by key (bin) field index
  public static final int INT_FIELD_IDX = 1; // count field index

  public Histograms() throws Exception {
    System.setProperty("vaultdb.setup", Utilities.getVaultDBRoot() + "/conf/setup.global");
    SystemConfiguration.resetConfiguration();
    SystemCatalog.resetInstance();
    ConnectionManager.reset();

    config = SystemConfiguration.getInstance();
    calciteConfig = config.getCalciteConfiguration();
  }

  private int aggregateBins(
          List<Tuple> tuples, int index, int binsToModify, int resultsPerBin, List<Tuple> newTuples) throws IOException {
    while (binsToModify > 0) {
      Tuple aggregatedTuple = new Tuple();
      Tuple currTuple = tuples.get(index);

      StringJoiner charFieldStringJoiner = new StringJoiner(", ", "[", "]");
      CharField currTupleCharField = (CharField) currTuple.getField(CHAR_FIELD_IDX);
      CharField charField = new CharField(currTupleCharField.getAttribute(), currTupleCharField.getSqlTypeName());

      IntField currTupleIntField = (IntField) currTuple.getField(INT_FIELD_IDX);
      IntField intField = new IntField(currTupleIntField.getAttribute(), currTupleIntField.getSqlTypeName());

      for (int i = index; i < resultsPerBin; i++) {
        CharField oldCharField = (CharField) tuples.get(i).getField(CHAR_FIELD_IDX);
        charFieldStringJoiner.add(oldCharField.value);
        IntField count = (IntField) tuples.get(i).getField(INT_FIELD_IDX);
        intField.addValue(count.value);
      }
      String charFieldString = charFieldStringJoiner.toString();
      charField.setValue(charFieldString);
      aggregatedTuple.addField(charField);
      aggregatedTuple.addField(intField);
      newTuples.add(aggregatedTuple);
      index += resultsPerBin;
      binsToModify--;
    }
    return index;
  }

  public ArrayList<QueryTable> getHistograms(
      String tableName, ArrayList<String> columnNames, int numBins) throws Exception {
    List<String> workers = ConnectionManager.getInstance().getAliceAndBob();
    ArrayList<QueryTable> finalHistogramList = new ArrayList<>();
    for (String col: columnNames){
      ArrayList<QueryTable> histogramListByWorkers = new ArrayList<>();
      for (String workerId: workers){
        QueryTable qt = getQueryTable(tableName, workerId, col);
        System.out.println(workerId + qt);
        histogramListByWorkers.add(qt);
      }
      HashMap<Field, Tuple> fieldTupleHashMap = new HashMap<>();
      for (QueryTable queryTable: histogramListByWorkers){
        for (Tuple tuple : queryTable.tuples()) {
          if (fieldTupleHashMap.containsKey(tuple.getField(CHAR_FIELD_IDX))) {
            Tuple aggregatedTuple = getAggregatedTuple(fieldTupleHashMap, tuple);
            fieldTupleHashMap.put(aggregatedTuple.getField(CHAR_FIELD_IDX), aggregatedTuple);
          } else {
            fieldTupleHashMap.put(tuple.getField(CHAR_FIELD_IDX), tuple);
          }
        }
      }
      List<Tuple> tupleList = new ArrayList<>(fieldTupleHashMap.values());
      QueryTable summedHistogram = new QueryTable(tupleList);
      finalHistogramList.add(resizeQueryTableToHistogram(numBins, summedHistogram));
    }
    return finalHistogramList;
  }

  private Tuple getAggregatedTuple(HashMap<Field, Tuple> fieldTupleHashMap, Tuple tuple) {
    Tuple aggregatedTuple = new Tuple();
    aggregatedTuple.addField(tuple.getField(CHAR_FIELD_IDX));

    IntField aCount =
            (IntField)
                    fieldTupleHashMap.get(tuple.getField(CHAR_FIELD_IDX)).getField(INT_FIELD_IDX);
    IntField bCount = (IntField) tuple.getField(INT_FIELD_IDX);

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
    return resizeQueryTableToHistogram(numBins, resultTable);
  }

  private QueryTable resizeQueryTableToHistogram(int numBins, QueryTable resultTable) throws Exception {
    if (numBins <= 0){
      throw new Exception("Number of bins in histogram must be greater than 0");
    }
    final int tupleCount = resultTable.tupleCount();
    if (tupleCount > numBins) {
      return getAggregatedTable(numBins, resultTable, tupleCount);
    } else if (tupleCount < numBins) {
      return getPaddedTable(numBins, resultTable);
    }
    return resultTable;
  }

  private QueryTable getAggregatedTable(int numBins, QueryTable resultTable, int tupleCount) throws Exception {
    final List<Tuple> tuples = resultTable.tuples();
    final List<Tuple> newTuples = new ArrayList<>();

    final int resultsPerBinMin = tupleCount / numBins;
    int binsToModifyMin = numBins - tupleCount % numBins;
    final int resultsPerBinMax = (int) Math.ceil(tupleCount / numBins * 1.0);
    int binsToModifyMax = tupleCount % numBins;
    int index = 0;
    index = aggregateBins(tuples, index, binsToModifyMin, resultsPerBinMin, newTuples);
    aggregateBins(tuples, index, binsToModifyMax, resultsPerBinMax, newTuples);
    return new QueryTable(newTuples);
  }

  private QueryTable getPaddedTable(int numBins, QueryTable resultTable) throws Exception {
    int tupleCount = resultTable.tupleCount();
    int numBinsToAdd = numBins - tupleCount;
    List<Tuple> tuples = resultTable.tuples();
    Tuple referenceTuple = tuples.get(0);
    CharField referenceTupleCharField = (CharField) referenceTuple.getField(CHAR_FIELD_IDX);
    IntField referenceTupleIntField = (IntField) referenceTuple.getField(INT_FIELD_IDX);
    for (int i = 0; i < numBinsToAdd; i++) {
        Tuple zeroTuple = new Tuple();

        CharField charField = new CharField(referenceTupleCharField.getAttribute(), referenceTupleCharField.getSqlTypeName());
        zeroTuple.addField(charField);

        IntField intField = new IntField(referenceTupleIntField.getAttribute(), referenceTupleIntField.getSqlTypeName());
        zeroTuple.addField(intField);
        tuples.add(zeroTuple);
    }
    return new QueryTable(tuples);
  }
}
