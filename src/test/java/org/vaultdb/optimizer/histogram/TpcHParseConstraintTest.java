package org.vaultdb.optimizer.histogram;

import java.util.List;

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
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.type.RelDataTypeFactory;
import org.apache.calcite.rel.type.RelDataTypeSystem;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexShuttle;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.sql.SqlBasicCall;
import org.apache.calcite.sql.SqlCall;
import org.apache.calcite.sql.SqlCreate;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.SqlSelect;
import org.apache.calcite.sql.ddl.SqlCheckConstraint;
import org.apache.calcite.sql.ddl.SqlCreateTable;
import org.apache.calcite.sql.parser.SqlParseException;
import org.apache.calcite.sql.parser.SqlParser;
import org.apache.calcite.sql.parser.ddl.SqlDdlParserImpl;
import org.apache.calcite.sql.util.SqlString;
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
		
	  public void testLineitemExpression() throws Exception  {
		
		// to dump all constraints at once.  Not used yet.
		// String tableSchema = super.TABLE_DEFINITIONS.get("lineitem");
		String constraintExpression = "(l_linenumber >= 1) AND (l_linenumber <= 7)";
		String tableName = "lineitem";
		
		RexNode predicate = sqlParser.parseTableConstraint(tableName, constraintExpression);
		logger.info("Parsed " + predicate);
				
	}
	  
	  public void testLineitemTableDeclaration() throws Exception {
		  
		String tableName = "lineitem";
		String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);
		logger.info("Parsing: " + tableDefinition);
		
		
		
		//RelNode tableNode = sqlParser.parseTableDefinition(tableDefinition);
		//logger.info("Table node: " + tableNode);

		SqlCreateTable tableNode =  sqlParser.parseTableDefinition(tableDefinition);
		List<SqlNode> tableOperands = tableNode.getOperandList();
		SqlNodeList columns = (SqlNodeList) tableOperands.get(1);
		
		
		logger.info("Table node: " + tableNode.getKind());

		
		for(SqlNode node : columns) {
			logger.info("Parsed column=" + node.getKind() );
			if(node instanceof SqlCheckConstraint) {
				SqlCheckConstraint constraint = (SqlCheckConstraint) node;
				List<SqlNode> operands = constraint.getOperandList();
				// operands[0] = name
				// operands[1] = SqlBasicCall
				// need to get from SqlBasicCall to RexNode
				SqlBasicCall expression = (SqlBasicCall) operands.get(1);
				String sqlExpression = expression.toSqlString(config.DIALECT).getSql();
				
				// delete excess quotes
				sqlExpression  = sqlExpression.replace("\"", "");
				logger.info("Parsing sql string: " + sqlExpression);

				RexNode rowExpression = sqlParser.parseTableConstraint(tableName, sqlExpression);
				logger.info("**********Parsed rexnode: " + rowExpression);

			}
		}

  
		
		

		
		
	  }

}
