package org.vaultdb.db.schema.constraints;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.jdbc.CalciteConnection;
import org.apache.calcite.jdbc.CalciteSchema;
import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.prepare.CalciteCatalogReader;
import org.apache.calcite.prepare.Prepare;
import org.apache.calcite.prepare.Prepare.PreparingTable;
import org.apache.calcite.rel.RelReferentialConstraint;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rel.type.RelRecordType;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Statistic;
import org.apache.calcite.schema.Table;
import org.apache.calcite.sql.SqlBasicCall;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.ddl.SqlCheckConstraint;
import org.apache.calcite.sql.ddl.SqlCreateTable;
import org.apache.calcite.util.ImmutableBitSet;
import org.apache.calcite.util.mapping.IntPair;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelDataTypeField.SecurityPolicy;

// keeps track of any constraints associated with a table in the DBMS
// needs to be initialized after constructor called for SystemCatalog
public class TableDefinition {
	
	String tableName;
	
	SystemConfiguration config;
	SystemCatalog catalog;
	
	
	List<List<SecureRelDataTypeField> > uniqueKeys; // primary keys or UNIQUE constraints, 
	/// can look up security type policy for RelDataTypeField  with Utilities.lookupAttribute
    
	List<RexNode> checkConstraints; // arbitrary expressions	
	List<RelReferentialConstraint> integrityConstraints; // foreign key relationships
	IntegrityConstraints integrityConstraintReferences;
	
	
	
	
	public TableDefinition(String aTableName, SystemCatalog aCatalog) throws Exception {

		// TODO: JMR, make this all derive from a single CREATE TABLE string
		
		// 1) grab unique, primary and foreign keys
		// 2) Populate a table of Map<Column, ColumnConstraints>

		tableName = aTableName;
		
		config = SystemConfiguration.getInstance();
		SchemaPlus sharedSchema = config.getPdfSchema();
		
		catalog = aCatalog; // needed to break dependency loop on initializing both of these things
		
		Table table = sharedSchema.getTable(tableName);
		Statistic statistic = table.getStatistic();
		
		JavaTypeFactory typeFactory = config.getCalciteConnection().getTypeFactory();

	    RelRecordType rowType = (RelRecordType) table.getRowType(typeFactory);
		List<RelDataTypeField> tableFields = rowType.getFieldList();

	    List<ImmutableBitSet> keyBitmasks = statistic.getKeys(); // each entry is a key - may be composite
		
	    uniqueKeys = new ArrayList<List<SecureRelDataTypeField> >();
		
	    for(ImmutableBitSet bits : keyBitmasks) { // for each key
	    	List<Integer> columns = bits.toList();
	    	List<SecureRelDataTypeField> aKey = new ArrayList<SecureRelDataTypeField>();
	    	
	    	for(Integer ordinal : columns) {
	    		RelDataTypeField field = tableFields.get(ordinal);
	    		SecurityPolicy policy = catalog.getPolicy(tableName, field.getName());
	    		SecureRelDataTypeField secureField = new SecureRelDataTypeField(field, policy, tableName);
	    		aKey.add(secureField);
	    	}
	    	
	    	uniqueKeys.add(aKey);
	    }
	    
		
	    integrityConstraints = statistic.getReferentialConstraints();
	    integrityConstraintReferences  = new IntegrityConstraints(tableName);
	    

	    
	    for(RelReferentialConstraint constraint : integrityConstraints) { // for each constraint
	    	int colCount = constraint.getNumColumns();
    		List<IntPair> pairs = constraint.getColumnPairs();
    		Map<SecureRelDataTypeField, SecureRelDataTypeField> referenceLookup = new HashMap<SecureRelDataTypeField, SecureRelDataTypeField>();
    		
	    	for(int i = 0; i < colCount; ++i) { // for each col in a constraint
	    		String srcTable = constraint.getSourceQualifiedName().get(i);
	    		Integer srcOrdinal = pairs.get(i).source;
	    		
	    		assert(srcTable.contentEquals(tableName)); // pointer belongs to the current table
	    		SecureRelDataTypeField secureSrcField = getSecureRelDataTypeField(srcTable, rowType.getFieldList().get(srcOrdinal));
	    		
	    		String targetTableName = constraint.getTargetQualifiedName().get(i);
	    		Integer targetOrdinal = pairs.get(i).target;
	    		Table targetTable = sharedSchema.getTable(targetTableName);

	    		RelDataTypeField targetField = targetTable.getRowType(typeFactory).getFieldList().get(targetOrdinal);
	    		
	    		SecureRelDataTypeField secureTargetField =  getSecureRelDataTypeField(targetTableName, targetField);
	    		referenceLookup.put(secureSrcField, secureTargetField);
	    	}
	    	integrityConstraintReferences.addConstraint(referenceLookup);

	    }

	    

	
	}
	
	
	private SecureRelDataTypeField getSecureRelDataTypeField(String tableName, RelDataTypeField f) {
		SecurityPolicy policy = catalog.getPolicy(tableName, f.getName());
		return new SecureRelDataTypeField(f, policy, tableName);
		
	}
	
	
	
	void populateCheckConstraints(String createTableStatement) throws Exception {
		
		SqlStatementParser sqlParser = new SqlStatementParser();
		
		SqlCreateTable tableNode =  sqlParser.parseTableDefinition(createTableStatement);
		List<SqlNode> tableOperands = tableNode.getOperandList();
		SqlNodeList columns = (SqlNodeList) tableOperands.get(1);
		
		

		checkConstraints = new ArrayList<RexNode>();
		
		for(SqlNode node : columns) {
		
			if(node instanceof SqlCheckConstraint) {
				SqlCheckConstraint constraint = (SqlCheckConstraint) node;
				List<SqlNode> operands = constraint.getOperandList();
				// operands[0] = name
				// operands[1] = SqlBasicCall
				// need to get from SqlBasicCall to RexNode
				SqlBasicCall expression = (SqlBasicCall) operands.get(1);
				String sqlExpression = expression.toSqlString(SystemConfiguration.DIALECT).getSql();
				
				// delete excess quotes
				sqlExpression  = sqlExpression.replace("\"", "");
				RexNode rowExpression = sqlParser.parseTableConstraint(tableName, sqlExpression);
				checkConstraints.add(rowExpression);
			
			}
		}
		


	  
	}


	public List<SecureRelDataTypeField> getUniqueKey(int i) {
		if(uniqueKeys.size() < i) 
			return uniqueKeys.get(i);
		
		return null;
	}


	public Map<SecureRelDataTypeField, SecureRelDataTypeField> getIntegrityConstraint(int i) {
		return integrityConstraintReferences.getConstaint(i);

	}
	

}
