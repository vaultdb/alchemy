package org.vaultdb.optimizer.histogram;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.prepare.PlannerImpl;
import org.apache.calcite.sql.SqlNode;
import org.apache.commons.lang.StringUtils;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

public class ParseConstraintTest  extends TpcHBaseTest {
	
		Map<String, String> tableDefinitions;
	
	  protected void setUp() throws Exception {
		  super.setUp();
		  
		  // schema definition taken with "pg_dump -s db_name > dst_file
		  
		  String schemaFile = Utilities.getVaultDBRoot()  + "/" + config.getProperty("schema-definition");
		  assert(schemaFile != null);
		  
		  tableDefinitions = new HashMap<String, String>();
		  
		  List<String> schemaDefinition = FileUtilities.readFile(schemaFile);
		  List<String>  tableDefinitions = new ArrayList<String>();
		  
		  String definition = "";
		  // delete all comments
		  for(String line : schemaDefinition) {
			  if(!line.startsWith("--") && !line.isBlank()) {
				definition += line;
			  }
			  
			  else if(line.contains(";")) {
				  // close out the definition, start a new one
				  // ignore if not a table declaration - e.g., index declaration
				  if(definition.toLowerCase().contains("create") && definition.toLowerCase().contains("table")) {
					  tableDefinitions.add(definition);
				  }
				  definition = "";
				  
			  }
		  }
		  
		 		  
		  for(String tableDefinition : tableDefinitions) {
			  logger.info(tableDefinition);
		  }
		  
		  
		  

		  
	  }
	
	public void testLineitem() throws Exception  {
		String constraint = new String("(((l_linenumber >= 1) AND (l_linenumber <= 7)))");
	
		
		PlannerImpl planner = new PlannerImpl(config.getCalciteConfiguration());
		SqlNode parseSql = planner.parse(constraint);  // don't validate it just yet
		
		
		
		logger.info("Parsed a " + parseSql.getClass());
		// SqlRexConvertlet converts SqlNode to RexNode
				
	}

}
