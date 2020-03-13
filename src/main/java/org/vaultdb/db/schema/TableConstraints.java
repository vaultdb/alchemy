package org.vaultdb.db.schema;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.calcite.adapter.java.JavaTypeFactory;
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
import org.apache.calcite.sql.validate.SqlValidatorUtil;
import org.apache.calcite.util.ImmutableBitSet;
import org.vaultdb.config.SystemConfiguration;

// keeps track of any constraints associated with a table in the DBMS
public class TableConstraints {
	
	// constraints from table definition
	List<List<RelDataTypeField> > keys; // primary keys or UNIQUE constraints
    List<RexNode> constraints; // arbitrary expressions	
	List<RelReferentialConstraint> integrityConstraints; // foreign key relationships

	
	// supporting vars
	Table baseTable;
	
	
	public TableConstraints(String tableName) throws Exception {

		// set up calcite config
		SystemConfiguration config = SystemConfiguration.getInstance();
		SchemaPlus sharedSchema = config.getPdfSchema();
		CalciteConnection calciteConnection = config.getCalciteConnection();
		Statistic statistic = baseTable.getStatistic();		
		JavaTypeFactory typeFactory = config.getCalciteConnection().getTypeFactory();

	    RelRecordType rowType = (RelRecordType) baseTable.getRowType(typeFactory);
		List<RelDataTypeField> tableFields = rowType.getFieldList();
		
		  final Prepare.CatalogReader catalogReader = new CalciteCatalogReader(
	    	        CalciteSchema.from(sharedSchema),
	    	        CalciteSchema.from(sharedSchema).path(null),
	    	        (JavaTypeFactory) typeFactory, calciteConnection.config());
	      
		// populate the member vars
		baseTable = sharedSchema.getTable(tableName);

	    integrityConstraints = statistic.getReferentialConstraints();
		
	    // extract keysets
	    List<ImmutableBitSet> keyBitmasks = statistic.getKeys();
		keys = new ArrayList<List<RelDataTypeField> >();
		
	    for(ImmutableBitSet bits : keyBitmasks) {
	    	List<Integer> columns = bits.toList();
	    	List<RelDataTypeField> keyFields = new ArrayList<RelDataTypeField>();
	    	
	    	for(Integer ordinal : columns) {
	    		keyFields.add(tableFields.get(ordinal));
	    	}
	    }
	    
	    
	    List<String> tableNames = new ArrayList<String>();
	    tableNames.add(tableName);
	    
	    PreparingTable prepTable = catalogReader.getTable(tableNames);
	    
	    
	    final ModifiableViewTable modifiableViewTable =  prepTable.unwrap(ModifiableViewTable.class);

	    // get additional constraints, e.g., domains/ranges of cols
	    Map<Integer, RexNode> constraintsMap = RelOptUtil.getColumnConstraints(modifiableViewTable, rowType, typeFactory);
	    
	    // TODO: check out RelOptUtil.inferViewPredicates​(Map<Integer,​RexNode> projectMap, List<RexNode> filters, RexNode constraint) for adding WHERE clause to constraints list
	    
	}

}
