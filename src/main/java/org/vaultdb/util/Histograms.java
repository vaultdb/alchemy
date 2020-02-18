package org.vaultdb.util;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.sql.type.SqlTypeName;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.vaultdb.codegen.sql.SqlGenerator;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.CharField;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.type.SecureRelDataTypeField;

import javax.management.Query;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringJoiner;
import java.util.stream.Collectors;

public class Histograms {

  protected SystemConfiguration config;
  protected FrameworkConfig calciteConfig;

  public static final int CHAR_FIELD_IDX = 0;
  public static final int INT_FIELD_IDX = 1;

  public Histograms() throws Exception {
    System.setProperty("vaultdb.setup", Utilities.getVaultDBRoot() + "/conf/setup.global");
    SystemConfiguration.resetConfiguration();
    SystemCatalog.resetInstance();
    ConnectionManager.reset();

    config = SystemConfiguration.getInstance();
    calciteConfig = config.getCalciteConfiguration();
  }
  // TODO: add getField indexes as constants instead of just magic numbers
  private int aggregateBins(
          List<Tuple> tuples, int index, int binsToModifyMin, int resultsPerBinMin, List<Tuple> newTuples) throws IOException {
    while (binsToModifyMin > 0) {
      Tuple aggregatedTuple = new Tuple();
      Tuple currTuple = tuples.get(index);

      StringJoiner charFieldStringJoiner = new StringJoiner(", ", "[", "]");
      CharField currTupleCharField = (CharField) currTuple.getField(CHAR_FIELD_IDX);
      CharField charField = new CharField(currTupleCharField.getAttribute(), currTupleCharField.getSqlTypeName());

      IntField currTupleIntField = (IntField) currTuple.getField(INT_FIELD_IDX);
      IntField intField = new IntField(currTupleIntField.getAttribute(), currTupleIntField.getSqlTypeName());

      for (int i = index; i < resultsPerBinMin; i++) {
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
      index += resultsPerBinMin;
      binsToModifyMin--;
    }
    return index;
  }

  public ArrayList<QueryTable> getHistogram(
      String tableName, ArrayList<String> columnNames, int numBins) throws Exception {
    if (numBins <= 0){
      throw new Exception("Number of bins in histogram must be greater than 0");
    }
    ArrayList<QueryTable> allQueryTables = new ArrayList<>();
    for (String col : columnNames) {
      RelBuilder builder = RelBuilder.create(calciteConfig);
//      String.format("SELECT COUNT(%s) FROM %s GROUP BY %s ORDER BY %s", col, tableName, col, col);
      RelNode node =
              builder
                      .scan(tableName)
                      .aggregate(builder.groupKey(col),
                              builder.count(false, "cnt"))
                      .sort(0)
                      .build();

      String query = SqlGenerator.getSql(node, config.DIALECT);
      System.out.println("Query: \n" + query);
      QueryTable resultTable = SqlQueryExecutor.query(query);

      final int tupleCount = resultTable.tupleCount();

      if (tupleCount > numBins) {
        final List<Tuple> tuples = resultTable.tuples();
        final List<Tuple> newTuples = new ArrayList<>();

        final int resultsPerBinMin = tupleCount / numBins;
        int binsToModifyMin = numBins - tupleCount % numBins;
        final int resultsPerBinMax = (int) Math.ceil(tupleCount / numBins * 1.0);
        int binsToModifyMax = tupleCount % numBins;
        int index = 0;
        index = aggregateBins(tuples, index, binsToModifyMin, resultsPerBinMin, newTuples);
        aggregateBins(tuples, index, binsToModifyMax, resultsPerBinMax, newTuples);
        QueryTable newTable = new QueryTable(newTuples);
        allQueryTables.add(newTable);

      } else if (tupleCount < numBins) {
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
        QueryTable paddedTable = new QueryTable(tuples);
        allQueryTables.add(paddedTable);
      } else if (tupleCount == numBins) {
        allQueryTables.add(resultTable);
      }
    }

    return allQueryTables;
  }
}
