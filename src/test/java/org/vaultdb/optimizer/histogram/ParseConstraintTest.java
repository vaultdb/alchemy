package org.vaultdb.optimizer.histogram;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.config.Lex;
import org.apache.calcite.prepare.PlannerImpl;
import org.apache.calcite.sql.SqlCreate;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.parser.SqlParser;
import org.apache.calcite.sql.parser.ddl.SqlDdlParserImpl;
import org.apache.calcite.sql.validate.SqlConformanceEnum;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

public class ParseConstraintTest  extends TpcHBaseTest {
	
		Map<String, SqlNode> tableDefinitions;
	
	  protected void setUp() throws Exception {
		  super.setUp();
		  
		  // schema definition taken with "pg_dump -s db_name > dst_file
		  
		  String schemaFile = Utilities.getVaultDBRoot()  + "/" + config.getProperty("schema-definition");
		  assert(schemaFile != null);


		  tableDefinitions = new HashMap<String, SqlNode>();
		  SqlParser.Config sqlParserConfig = SqlParser.configBuilder()
				    .setParserFactory(SqlDdlParserImpl.FACTORY)
				    .setConformance(SqlConformanceEnum.MYSQL_5) // psql not available
				    .setLex(Lex.MYSQL)
				    .build();
		  
		  List<String> schemaDefinition = FileUtilities.readFile(schemaFile);
		  List<String>  tableDefinitions = new ArrayList<String>();
		  
		  String sqlQuery = "";
		  // delete all comments
		  for(String line : schemaDefinition) {
			  if(!line.startsWith("--") && !line.isBlank()) {
				sqlQuery += line;
			  
				// assume no additional text after ";
				if(line.contains(";")) { 
		
					if(sqlQuery.toLowerCase().contains("create table")) {
					
							sqlQuery.replace(';',' ');
							logger.info("Parsing " + sqlQuery);
							SqlParser parser = SqlParser.create(sqlQuery, sqlParserConfig);
							//SqlNode statement = planner.parse(definition);
							SqlNode statement = parser.parseStmt();
							if(statement.getKind() == SqlKind.CREATE_TABLE)  {
								SqlCreate tableDefinition =  (SqlCreate) statement;
								logger.info("Parsed " + sqlQuery);

								//tableDefinitions.add(definition);
							}// create table
						} // create table, pt 1 
					 sqlQuery = "";
				} // end ";" case
			  } // end "not comment" case
		  }
		  
		 		  
		  for(String tableDefinition : tableDefinitions) {
			  logger.info(tableDefinition);
		  }
		  
		  
		  

		  
	  }
	
	public void testLineitem() throws Exception  {
		String constraint = new String("(((l_linenumber >= 1) AND (l_linenumber <= 7)))");
	
		// See http://users.eecs.northwestern.edu/~jennie/documentation/calcite-javadoc/org/apache/calcite/sql2rel/SqlNodeToRexConverter.html
		// for getting from SqlNode to RexNode
		
		// how to parse an expression to  SqlNode?  It's a SqlCall more precisely
		
		PlannerImpl planner = new PlannerImpl(config.getCalciteConfiguration());
		SqlNode parseSql = planner.parse(constraint);  // don't validate it just yet
		
		
		
		logger.info("Parsed a " + parseSql.getClass());
		// SqlRexConvertlet converts SqlNode to RexNode
				
	}

}
