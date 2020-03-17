package org.vaultdb.optimizer.histogram;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.vaultdb.BaseTest;
import org.vaultdb.codegen.sql.SqlGenerator;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.Field;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class CollectHistogramTest extends BaseTest {

  public void testCollectDiagnoses() throws Exception {

    final FrameworkConfig calciteConfig = config.getCalciteConfiguration();
    RelBuilder builder = RelBuilder.create(calciteConfig);
    RelNode node =
        builder
            .scan("diagnoses")
            .aggregate(builder.groupKey("clean_icd9"), builder.count(false, "cnt"))
            .build();

    String query = SqlGenerator.getSql(node, config.DIALECT);
    System.out.println("Query: \n" + query);

    String aId = ConnectionManager.getInstance().getAlice();
    String bId = ConnectionManager.getInstance().getBob();

    SecureRelRecordType dstSchema = Utilities.getOutSchemaFromSql(query);
    QueryTable aTable = SqlQueryExecutor.query(query, dstSchema, aId);
    QueryTable bTable = SqlQueryExecutor.query(query, dstSchema, bId);

//    QueryTable completeTable = SqlQueryExecutor.query(query);

    HashMap<Field, Tuple> fieldTupleHashMap = new HashMap<>();

    for (Tuple a : aTable.tuples()) {
      fieldTupleHashMap.put(a.getField(0), a);
    }
    for (Tuple b: bTable.tuples()){
      if (fieldTupleHashMap.containsKey(b.getField(0))){
        Tuple aggregatedTuple = new Tuple();
        aggregatedTuple.addField(b.getField(0));

        IntField aCount = (IntField) fieldTupleHashMap.get(b.getField(0)).getField(1);
        IntField bCount = (IntField) b.getField(1);

        IntField aggregatedCount = new IntField(bCount.getAttribute(), bCount.getSqlTypeName());
        aggregatedCount.setValue(aCount.getValue() + bCount.getValue());
        aggregatedTuple.addField(aggregatedCount);

        fieldTupleHashMap.put(aggregatedTuple.getField(0), aggregatedTuple);
      } else {
        fieldTupleHashMap.put(b.getField(0), b);
      }
    }
    List<Tuple> tupleList = new ArrayList<>(fieldTupleHashMap.values());
    QueryTable summedHistogram = new QueryTable(tupleList);

    System.out.println("A table: " + aTable);
    System.out.println("B table: " + bTable);
    System.out.println("Summed histogram: " + summedHistogram);
  }

  public void testCreateGroupByOrderByQuery() throws Exception {
    final FrameworkConfig calciteConfig = config.getCalciteConfiguration();
    RelBuilder builder = RelBuilder.create(calciteConfig);
    RelNode node =
            builder
                    .scan("diagnoses")
                    .aggregate(builder.groupKey("clean_icd9"),
                            builder.count(false, "cnt"))
                    .sort(0)
                    .build();

    String query = SqlGenerator.getSql(node, config.DIALECT);
    System.out.println("Query: \n" + query);
  }

}
