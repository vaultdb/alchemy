package org.vaultdb.optimizer.histogram;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.config.Lex;
import org.apache.calcite.jdbc.CalciteConnection;
import org.apache.calcite.jdbc.CalciteSchema;
import org.apache.calcite.prepare.CalciteCatalogReader;
import org.apache.calcite.prepare.PlannerImpl;
import org.apache.calcite.prepare.Prepare;
import org.apache.calcite.rel.type.RelDataTypeFactory;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.sql.SqlCall;
import org.apache.calcite.sql.SqlCreate;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.sql.SqlNode;
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
import org.vaultdb.parser.SqlStatementParser.LocalValidatorImpl;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

public class TpcHParseConstraintTest  extends TpcHBaseTest {
	
	SqlStatementParser sqlParser;
	
	SqlParser.Config sqlParserConfig;
	
	  protected void setUp() throws Exception {
		  super.setUp();
		  
		  sqlParserConfig = SqlParser.configBuilder()
				    .setParserFactory(SqlDdlParserImpl.FACTORY)
				    .setConformance(SqlConformanceEnum.MYSQL_5) // psql not available - blasphemy!
				    .setLex(Lex.MYSQL)
				    .build();
		  
			FrameworkConfig calciteConfig = config.getCalciteConfiguration();
			CalciteConnection calciteConnection = config.getCalciteConnection();
			SchemaPlus sharedSchema = config.getPdfSchema();
			
			Planner planner = new PlannerImpl(calciteConfig);

		  
	      final RelDataTypeFactory typeFactory = planner.getTypeFactory();
		     

	      final Prepare.CatalogReader catalogReader = new CalciteCatalogReader(
	    	        CalciteSchema.from(sharedSchema),
	    	        CalciteSchema.from(sharedSchema).path(null),
	    	        (JavaTypeFactory) typeFactory, calciteConnection.config());

	          
	      sqlParser = new SqlStatementParser();
	      
	    
	   
	  }
		
	  public void testLineitem() throws Exception  {
		
		// to dump all constraints at once.  Not used yet.
		// String tableSchema = super.TABLE_DEFINITIONS.get("lineitem");
		String constraintExpression = "(((l_linenumber >= 1) AND (l_linenumber <= 7)))";
		
		//SqlParser parser = SqlParser.create(constraintExpression, sqlParserConfig);
		//SqlNode expression = parser.parseExpression();
		

		RexNode expression = sqlParser.parseExpression(constraintExpression);
		
		logger.info("Parsed a " + expression.getClass());
		
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
