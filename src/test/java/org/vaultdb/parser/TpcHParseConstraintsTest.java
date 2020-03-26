package org.vaultdb.parser;

import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.rex.RexNode;
import org.apache.calcite.sql.SqlBasicCall;
import org.apache.calcite.sql.SqlIdentifier;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.ddl.SqlCheckConstraint;
import org.apache.calcite.sql.ddl.SqlCreateTable;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.parser.SqlStatementParser;

import com.google.common.collect.ImmutableList;

public class TpcHParseConstraintsTest  extends TpcHBaseTest {
	
	SqlStatementParser sqlParser;
	
	
	  protected void setUp() throws Exception {
		  super.setUp();
		       

	      
	          
	      sqlParser = new SqlStatementParser();
	      
	    
	   
	  }
		
	  public void testLineitemExpression() throws Exception  {
		
		// to dump all constraints at once.  Not used yet.
		// String tableSchema = super.TABLE_DEFINITIONS.get("lineitem");
		String constraintExpression = "(l_linenumber >= 1) AND (l_linenumber <= 7)";
		String tableName = "lineitem";
		
		RexNode predicate = sqlParser.parseTableConstraint(tableName, constraintExpression);
		logger.info("Parsed " + predicate);
		assertEquals(predicate.toString(), "AND(<=(1, $3), >=(7, $3))");
				
	}

	  public void testCustomerTableDeclaration() throws Exception {
		  
		  String tableName = "customer";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);
		  
			List<RexNode> customerConstraints = extractTableConstraints(tableName, tableDefinition);
			final List<String> expectedRexNodes =   ImmutableList.of(					
					"AND(<=($5, 9999.99:DECIMAL(6, 2)), >=($5, CAST('-999.99'):DECIMAL(19, 0) NOT NULL))",
                    "AND(<=(1, $0), <=($0, 15000))",
                    "OR(=($6, 'BUILDING'), =($6, 'FURNITURE'), =($6, 'HOUSEHOLD'), =($6, 'MACHINERY'), =($6, 'AUTOMOBILE'))",
                    "AND(<=(0, $3), <=($3, 24))"
			);
			
			List<String> observedRexNodes = new ArrayList<String>();
			for(RexNode r : customerConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
	  }
	  
	  public void testLineitemTableDeclaration() throws Exception {
		  
		  String tableName = "lineitem";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		List<RexNode> lineitemConstraints = extractTableConstraints(tableName, tableDefinition);
		final List<String> expectedRexNodes = ImmutableList.of(
				"AND(<=($6, 0.10:DECIMAL(3, 2)), >=($6, 0.00:DECIMAL(3, 2)))",
				"AND(<=(1, $3), >=(7, $3))",
				"OR(=($9, 'F'), =($9, 'O'))",
				"AND(<=(1, $1), <=($1, 20000))",
				"AND(<=(1, $4), <=($4, 50))",
				"OR(=($8, 'A'), =($8, 'N'), =($8, 'R'))",
				"OR(=($13, 'NONE'), =($13, 'COLLECT COD'), =($13, 'TAKE BACK RETURN'), =($13, 'DELIVER IN PERSON'))",
				"OR(=($14, 'AIR'), =($14, 'FOB'), =($14, 'MAIL'), =($14, 'RAIL'), =($14, 'SHIP'), =($14, 'TRUCK'), =($14, 'REG AIR'))",
				"AND(<=($7, 0.08:DECIMAL(3, 2)), >=($7, 0.00:DECIMAL(3, 2)))");
		
		List<String> observedRexNodes = new ArrayList<String>();

		for(RexNode r : lineitemConstraints) {
			observedRexNodes.add(r.toString());
		}
		
		assertEquals(expectedRexNodes, observedRexNodes);	
	
	  }
	  

	  public void testNationTableDeclaration() throws Exception {
		  String tableName = "nation";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> nationConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : nationConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  
		  
	  }
	  

	  public void testOrdersTableDeclaration() throws Exception {
		  String tableName = "orders";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> checkConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : checkConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  

	  }
	  

	  public void testPartTableDeclaration() throws Exception {
		  String tableName = "part";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> checkConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : checkConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  

	  }
	  
	  public void testPartsuppDeclaration() throws Exception {

		  String tableName = "partsupp";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> checkConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : checkConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  

	  }
	    
	  
	  public void testRegionTableDeclaration() throws Exception {
		  String tableName = "region";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> checkConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : checkConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  

	  }
	  
	  public void testSupplierTableDeclaration() throws Exception {
		
		  String tableName = "supplier";
		  String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

		  List<RexNode> checkConstraints = extractTableConstraints(tableName, tableDefinition);


		  
		  List<String> observedRexNodes = new ArrayList<String>();

		  // TODO: add any expected constraints and test for them
		  final List<String> expectedRexNodes = ImmutableList.of();
		  
			for(RexNode r : checkConstraints) {
				observedRexNodes.add(r.toString());
			}
			
			assertEquals(expectedRexNodes, observedRexNodes);	
		  

	  }
	  
	  
	  List<RexNode> extractTableConstraints(String tableName, String tableDefinition) throws Exception {
		  
			
			
			
			//RelNode tableNode = sqlParser.parseTableDefinition(tableDefinition);
			//logger.info("Table node: " + tableNode);

			SqlCreateTable tableNode =  sqlParser.parseTableDefinition(tableDefinition);
			List<SqlNode> tableOperands = tableNode.getOperandList();
			SqlNodeList columns = (SqlNodeList) tableOperands.get(1);
			
			
	
			List<RexNode> rowExpressions = new ArrayList<RexNode>();
			
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
					rowExpressions.add(rowExpression);
				
				}
			}
			

		  return rowExpressions;
	  }

}
