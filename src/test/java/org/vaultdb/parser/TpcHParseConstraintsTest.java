package org.vaultdb.parser;

import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.rex.RexNode;
import org.apache.calcite.sql.SqlBasicCall;
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
		List<RexNode> customerConstraints = extractTableConstraints("customer");
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
		List<RexNode> lineitemConstraints = extractTableConstraints("lineitem");
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

	// TODO: add back in n_name_domain constraint
	public void testNationTableDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("nation");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"AND(<=(0, $0), <=($0, 24))",
				"AND(<=(0, $2), >=(4, $2))"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			System.out.println(r.toString());
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}


	public void testOrdersTableDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("orders");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"AND(<=($4, CAST('1998-12-31'):DATE NOT NULL), >=($4, CAST('1992-01-01'):DATE NOT NULL))",
				"AND(<=(1, $0), <=($0, 600000))",
				"OR(=($5, '5-LOW'), =($5, '2-HIGH'), =($5, '1-URGENT'), =($5, '3-MEDIUM'), =($5, '4-NOT SPECIFIED'))",
				"OR(=($2, 'F'), =($2, 'O'), =($2, 'P'))",
				"=(0, $7)"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}


	public void testPartTableDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("part");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"AND(<=(1, $5), <=($5, 50))"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}

	public void testPartsuppDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("partsupp");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"AND(<=(1, $2), <=($2, 9999))",
				"AND(>=($3, 1.00:DECIMAL(3, 2)), <=($3, 1000.00:DECIMAL(6, 2)))"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			System.out.println(r.toString());
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}


	public void testRegionTableDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("region");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"OR(=($1, 'ASIA'), =($1, 'AFRICA'), =($1, 'EUROPE'), =($1, 'AMERICA'), =($1, 'MIDDLE EAST'))",
				"AND(<=(0, $0), >=(4, $0))"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}

	public void testSupplierTableDeclaration() throws Exception {
		List<RexNode> customerConstraints = extractTableConstraints("supplier");
		final List<String> expectedRexNodes =   ImmutableList.of(
				"AND(<=($5, 9999.99:DECIMAL(6, 2)), >=($5, CAST('-999.99'):DECIMAL(19, 0) NOT NULL))",
				"AND(<=(0, $3), <=($3, 24))",
				"AND(<=(1, $0), <=($0, 1000))"
		);

		List<String> observedRexNodes = new ArrayList<String>();
		for(RexNode r : customerConstraints) {
			observedRexNodes.add(r.toString());
		}

		assertEquals(expectedRexNodes, observedRexNodes);
	}

	List<RexNode> extractTableConstraints(String tableName) throws Exception {

		String tableDefinition = super.TABLE_DEFINITIONS.get(tableName);

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
