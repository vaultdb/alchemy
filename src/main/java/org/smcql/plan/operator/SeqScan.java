package org.smcql.plan.operator;

import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.adapter.jdbc.JdbcTableScan;
import org.apache.calcite.sql.type.SqlTypeName;
import org.apache.calcite.util.Pair;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.db.schema.SystemCatalog;
import org.smcql.executor.config.ExecutionMode;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.plan.SecureRelNode;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;

public class SeqScan extends Operator {
	
	SecureRelRecordType inSchema;
	String tableName;

	public SeqScan(String name, SecureRelNode src, Operator ... children ) throws Exception {
		super(name, src, children);
		
		JdbcTableScan scan = (JdbcTableScan) src.getPhysicalNode().baseRelNode.getRelNode();
		tableName = scan.getTable().getQualifiedName().get(0);

		executionMode = new ExecutionMode();
		executionMode.distributed = false;
		executionMode.oblivious = false;
		boolean replicatedTable = SystemCatalog.getInstance().isReplicated(tableName);
		executionMode.replicated = replicatedTable;
		
		sliceAgnostic = true;
	}
	
	public SecureRelRecordType getInSchema() {
		return baseRelNode.getSchema();
	}
	
	@Override
	public SecureRelRecordType getSchema(boolean isSecureLeaf) {
		SecureRelRecordType schema = baseRelNode.getSchema();
		return schema;
	}
	
	@Override
	public void initializeStatistics() {
		// infer from base relation
		baseRelNode.getSchema().initializeStatistics();
	}
	
	private List<String> getOrderableFields() {
		List<String> fieldNames = new ArrayList<String>();
		Operator parent = this.parent;
		while (parent != null) {
			if (parent instanceof Project) {
				for (SecureRelDataTypeField field : parent.getSchema().getAttributes()) {
					SqlTypeName type = field.getType().getSqlTypeName();
					
					if ((SqlTypeName.NUMERIC_TYPES.contains(type) || SqlTypeName.DATETIME_TYPES.contains(type)) && !fieldNames.contains(field.getName())) 
						fieldNames.add(field.getName());
				}
			}
			
			parent = parent.getParent();
		}
		return fieldNames;
	}
	
	@Override
	public List<SecureRelDataTypeField> secureComputeOrder() {
		List<SecureRelDataTypeField> result = new ArrayList<SecureRelDataTypeField>();
		for (String name: getOrderableFields()) {
			for (SecureRelDataTypeField field : this.getSchema().getAttributes()) {
				if (field.getName().equals(name))
					result.add(field);
			}
		}
		
		return result;
	}

	@Override
	public void inferExecutionMode() {
		return; // done in constructor
	}

	
	
	@Override
	public int getEstimatedCardinality() {
		
		//This errors out
		//SliceKeyDefinition s = getSliceKey();
		
		SecureRelDataTypeField keyField = getSliceKey().getAttributes().get(0);
		//String table = keyField.getStoredTable();
		
		//String attribute = keyField.getStoredAttribute();
		//String query = "SELECT COUNT(*) FROM <TABLENAME>";
		//SqlQueryExecutor qe = new SqlQueryExecutor();
		//QueryTable rawStats = qe.plainQuery(inSchema, query);
		//List<Tuple> tuples = rawStats.tuples();
		//return tuples.size();
		
		return 1;
	}
	
	@Override
	public Pair<Long, Long> obliviousCardinality() {

			return catalog.getTableCardinalities(tableName);
		
	}
	
};
