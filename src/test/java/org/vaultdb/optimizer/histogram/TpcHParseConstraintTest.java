package org.vaultdb.optimizer.histogram;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.config.Lex;
import org.apache.calcite.jdbc.CalciteConnection;
import org.apache.calcite.jdbc.CalciteSchema;
import org.apache.calcite.jdbc.JavaTypeFactoryImpl;
import org.apache.calcite.prepare.CalciteCatalogReader;
import org.apache.calcite.prepare.PlannerImpl;
import org.apache.calcite.prepare.Prepare;
import org.apache.calcite.prepare.Prepare.CatalogReader;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.type.RelDataTypeFactory;
import org.apache.calcite.rel.type.RelDataTypeSystem;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexShuttle;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.sql.SqlCall;
import org.apache.calcite.sql.SqlCreate;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlSelect;
import org.apache.calcite.sql.parser.SqlParser;
import org.apache.calcite.sql.parser.ddl.SqlDdlParserImpl;
import org.apache.calcite.sql.validate.SqlConformanceEnum;
import org.apache.calcite.sql.validate.SqlValidator;
import org.apache.calcite.sql2rel.SqlNodeToRexConverter;
import org.apache.calcite.sql2rel.SqlNodeToRexConverterImpl;
import org.apache.calcite.sql2rel.SqlRexConvertlet;
import org.apache.calcite.sql2rel.SqlRexConvertletTable;
import org.apache.calcite.sql2rel.SqlToRelConverter;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.Planner;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

public class TpcHParseConstraintTest  extends TpcHBaseTest {
	
	SqlStatementParser sqlParser;
	
	SqlParser.Config sqlParserConfig;
	SchemaPlus sharedSchema;
	
	  protected void setUp() throws Exception {
		  super.setUp();
		  
		  sqlParserConfig = SqlParser.configBuilder()
				    .setParserFactory(SqlDdlParserImpl.FACTORY)
				    .setConformance(SqlConformanceEnum.MYSQL_5) // psql not available - blasphemy!
				    .setLex(Lex.MYSQL)
				    .build();
		  
			sharedSchema = config.getPdfSchema();
			
		     

	      
	          
	      sqlParser = new SqlStatementParser();
	      
	    
	   
	  }
		
	  public void testLineitem() throws Exception  {
		
		// to dump all constraints at once.  Not used yet.
		// String tableSchema = super.TABLE_DEFINITIONS.get("lineitem");
		String constraintExpression = "(l_linenumber >= 1) AND (l_linenumber <= 7)";
		String tableName = "lineitem";
		
		
		// hack to give it context for mapping InputRefs to cols
		String constraintQuery = "SELECT * FROM " + tableName + " WHERE " + constraintExpression; 		
		SqlSelect query = (SqlSelect) sqlParser.parseSQL(constraintQuery);
		
		SqlNode predicate = query.getWhere();
		logger.info("Parsed a " + predicate.getClass() + " " + predicate.getKind());
		
		
		
		final SqlValidator validator = sqlParser.getValidator();
		final CalciteConnection calciteConnection = config.getCalciteConnection();
		
		
		
		RelDataTypeSystem typeSystem =
		        calciteConnection.config().typeSystem(RelDataTypeSystem.class,
		            RelDataTypeSystem.DEFAULT);
		final RelDataTypeFactory typeFactory = new JavaTypeFactoryImpl(typeSystem);

		    
		
		final CatalogReader catalogReader = new CalciteCatalogReader(
    	        CalciteSchema.from(sharedSchema),
    	        CalciteSchema.from(sharedSchema).path(null),
    	        (JavaTypeFactory) typeFactory, calciteConnection.config());

		assert(typeFactory != null);
		
		SqlToRelConverter sqlToRel = sqlParser.createSqlToRelConverter(validator, catalogReader, typeFactory);
		
		
		RelRoot queryTree = sqlToRel.convertQuery(query, true, true);
		RelNode rootNode = queryTree.rel;
		
		RexShuttle shuttle = new RexShuttle();
		
		List<RexNode> rowExpressions = rootNode.accept(shuttle);
		
		logger.info("Parsed a " + queryTree.getClass() + queryTree.rel.getClass());

		//SqlParser parser = SqlParser.create(constraintExpression, sqlParserConfig);
		//SqlNode expression = parser.parseExpression();
		

		//RexNode expression = sqlParser.parseExpression(constraintExpression);
		
		
		//SqlRexConvertletTable tableDefs = config.getCalciteConfiguration().getConvertletTable();

		
		//RexNode rowExpression =  converter.convertExpression(expression);
		//SqlNodeToRexConverter converter = new SqlNodeToRexConverterImpl(tableDefs);
		 //= tableDef.convertCall(null, expression);
		// SqlRexConvertlet converts SqlNode to RexNode		
		// See http://users.eecs.northwestern.edu/~jennie/documentation/calcite-javadoc/org/apache/calcite/sql2rel/SqlNodeToRexConverter.html
		// for more
		
		// how to parse an expression to  SqlNode?  It's a SqlCall more precisely
		
		//PlannerImpl planner = new PlannerImpl(config.getCalciteConfiguration());
		//SqlNode parseSql = planner.parse(constraint);  // don't validate it just yet
		
		
		
				
	}

}
