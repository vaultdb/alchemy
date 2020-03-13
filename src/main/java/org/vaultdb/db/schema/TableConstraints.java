package org.vaultdb.db.schema;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.avatica.Meta.DatabaseProperty;
import org.apache.calcite.jdbc.CalciteConnection;
import org.apache.calcite.jdbc.CalciteSchema;
import org.apache.calcite.jdbc.CalciteSchema.TableEntry;
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
import org.apache.calcite.schema.impl.ModifiableViewTable;
import org.apache.calcite.sql.validate.SqlValidatorTable;
import org.apache.calcite.sql.validate.SqlValidatorUtil;
import org.apache.calcite.util.ImmutableBitSet;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.type.SecureRelDataTypeField;

// keeps track of any constraints associated with a table in the DBMS
public class TableConstraints {
	
	// constraints from table definition
	List<List<SecureRelDataTypeField> > keys; // primary keys or UNIQUE constraints
    List<RexNode> constraints; // arbitrary expressions	
	List<RelReferentialConstraint> integrityConstraints; // foreign key relationships

	
	// supporting vars
	Table baseTable;
	

	
	public TableConstraints(String tableName) throws Exception {

		SystemConfiguration config = SystemConfiguration.getInstance();
		Logger logger = config.getLogger();
	
		/*
		String schemaFile = config.getProperty("schema-definition");
		if(schemaFile == null) {
			logger.info("No schema definition file found! Add it to the SystemConfiguration file.");
			return; // no constraints
		}
		
		
		// scan the table to find definition for tableName
		
		
			
     	Connection connection = workerConfig.getDbConnection();
		DatabaseMetaData dbMetadata = connection.getMetaData();
		String dbName = workerConfig.dbName;
		
		ResultSet foreignKeys = dbMetadata.getImportedKeys(dbName, "public", tableName);
		System.out.println("Foreign keys: " + resultSetToString(foreignKeys));

		
		ResultSet primaryKeys = dbMetadata.getPrimaryKeys(dbName, "public", tableName);
		System.out.println("Primary keys: " + resultSetToString(primaryKeys));

		
		String[] tableTypes = {"SYSTEM TABLE", "TABLE"};
		ResultSet resultSet = dbMetadata.getTables(null, null, "%", tableTypes);
		System.out.println("tables: " + resultSetToString(resultSet));*/



		
		// set up calcite config
		SchemaPlus sharedSchema = config.getPdfSchema();
		CalciteConnection calciteConnection = config.getCalciteConnection();
		JavaTypeFactory typeFactory = config.getCalciteConnection().getTypeFactory();


		baseTable = sharedSchema.getTable(tableName);
		Statistic statistic = baseTable.getStatistic();		
			    RelRecordType rowType = (RelRecordType) baseTable.getRowType(typeFactory);
		List<RelDataTypeField> tableFields = rowType.getFieldList();
		
		  final Prepare.CatalogReader catalogReader = new CalciteCatalogReader(
	    	        CalciteSchema.from(sharedSchema),
	    	        CalciteSchema.from(sharedSchema).path(null),
	    	        (JavaTypeFactory) typeFactory, calciteConnection.config());
	      

	    integrityConstraints = statistic.getReferentialConstraints();
		
	    // extract keysets
	    List<ImmutableBitSet> keyBitmasks = statistic.getKeys();
		keys = new ArrayList<List<SecureRelDataTypeField> >();
		
	    for(ImmutableBitSet bits : keyBitmasks) {
	    	List<Integer> columns = bits.toList();
	    	List<RelDataTypeField> keyFields = new ArrayList<RelDataTypeField>();
	    	
	    	for(Integer ordinal : columns) {
	    		keyFields.add(tableFields.get(ordinal));
	    	}
	    }
	    


		
		// CAUTION: posgresql-specific
		String constraintsQuery = "select pgc.conname as constraint_name,\n" + 
				"       ccu.table_name,\n" + 
				"       ccu.column_name,\n" + 
				"       pg_get_constraintdef(pgc.oid) definition\n" + 
				"from pg_constraint pgc\n" + 
				"join pg_namespace nsp on nsp.oid = pgc.connamespace\n" + 
				"join pg_class  cls on pgc.conrelid = cls.oid\n" + 
				"left join information_schema.constraint_column_usage ccu\n" + 
				"          on pgc.conname = ccu.constraint_name\n" + 
				"          and nsp.nspname = ccu.constraint_schema\n" + 
				"where contype ='c'\n" + 
				" AND ccu.table_name='" + tableName + "' \n" + 
				"order by pgc.conname";
		
		logger.info("Constraints query: " + constraintsQuery);
		QueryTable constraints = SqlQueryExecutor.query(constraintsQuery);
		

		logger.info("Table checks: " + constraints);
	}
	
	
	
	String resultSetToString(ResultSet result) throws SQLException {
		   ResultSetMetaData rsmd = result.getMetaData();
		   int columnCount = rsmd.getColumnCount();
		   
		   String printed = new String();
		   while (result.next()) {
		       for (int i = 1; i <= columnCount; i++) {
		           if (i > 1) System.out.print(",  ");
		           String columnValue = result.getString(i);
		           printed += columnValue + " " + rsmd.getColumnName(i);
		       }
		      printed += "\n";
		   }
		   
		   return printed;
	}
	
	

}
