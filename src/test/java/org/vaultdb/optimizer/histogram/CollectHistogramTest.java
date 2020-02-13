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

public class CollectHistogramTest  extends  BaseTest {
	
	
	public void testCollectDiagnoses() throws Exception {
		
		final FrameworkConfig calciteConfig = config.getCalciteConfiguration();
		RelBuilder builder = RelBuilder.create(calciteConfig);
		 RelNode node = builder
				  .scan("diagnoses")
				  .aggregate(builder.groupKey("clean_icd9"),
				      builder.count(false, "cnt")).build();
		 
		 
		 String query =  SqlGenerator.getSql(node, config.DIALECT);
		 System.out.println("Query " + query);
		 
		 
		 String aId = ConnectionManager.getInstance().getAlice();
		 String bId = ConnectionManager.getInstance().getBob();
		 
		
		 
		 SecureRelRecordType dstSchema = Utilities.getOutSchemaFromSql(query);
		 QueryTable aTable =  SqlQueryExecutor.query(query, dstSchema, aId);		 
		 QueryTable bTable =  SqlQueryExecutor.query(query, dstSchema, bId);
		 /*QueryTable summedHistogram = new QueryTable(aTable.getSchema());
		 
		 for (Tuple a :  aTable.tuples()) {
				IntField summedCount = (IntField) a.getField(1);
				
			 for (Tuple b :  bTable.tuples()) {
				if(b.getField(0) == a.getField(0)) {
					IntField bCount = (IntField) b.getField(1);
					summedCount.value += bCount.value;
					
					}
				}
			 a.setField(1, summedCount);
			 }
		 
		 */
		 
		 System.out.println("A table: " + aTable);
		 System.out.println("B table: " + bTable);
		 
		 } 

}
