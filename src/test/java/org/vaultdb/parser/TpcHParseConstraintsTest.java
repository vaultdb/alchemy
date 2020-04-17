package org.vaultdb.parser;

import java.lang.reflect.Array;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.sql.SqlBasicCall;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.ddl.SqlCheckConstraint;
import org.apache.calcite.sql.ddl.SqlCreateTable;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.codegen.smc.operator.support.RexFlattener;
import org.vaultdb.codegen.sql.SqlGenerator;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.db.schema.constraints.ColumnDefinition;
import org.vaultdb.db.schema.constraints.ColumnDefinitionFactory;
import org.vaultdb.db.schema.constraints.ConstraintVisitor;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.parser.SqlStatementParser;

import com.google.common.collect.ImmutableList;
import org.vaultdb.type.SecureRelDataTypeField;

import org.vaultdb.db.schema.constraints.TableDefinition;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.RexNodeUtilities;

public class TpcHParseConstraintsTest  extends TpcHBaseTest {

	SqlStatementParser sqlParser;
	protected FrameworkConfig calciteConfig;

	protected void setUp() throws Exception {
		super.setUp();
		sqlParser = new SqlStatementParser();
		calciteConfig = config.getCalciteConfiguration();
	}

	public void testConstraintVisitorSchemaConstructor() throws Exception {
		String tableName = "customer";
		List<RexNode> customerConstraints = extractTableConstraints("customer");

		// correct column definitions
		List<ColumnDefinition> correctColumnDefinitions = new ArrayList<>(customerConstraints.size());
		// "AND(<=($5, 9999.99:DECIMAL(6, 2)), >=($5, CAST('-999.99'):DECIMAL(19, 0) NOT NULL))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, -999.99, 9999.99, -1L));
		// "AND(<=(1, $0), <=($0, 15000))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, 1, 15000, -1L));
		// "OR(=($6, 'BUILDING'), =($6, 'FURNITURE'), =($6, 'HOUSEHOLD'), =($6, 'MACHINERY'), =($6, 'AUTOMOBILE'))"
		Set<Object> domain = new HashSet<>(Arrays. asList("MACHINERY", "AUTOMOBILE", "BUILDING", "FURNITURE", "HOUSEHOLD"));
		correctColumnDefinitions.add(new ColumnDefinition(null, 5L, -1L, domain, null, null, -1L));
		// "AND(<=(0, $3), <=($3, 24))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, 0, 24, -1L));

		// step 0: use rel builder to get schema
		RelBuilder builder = RelBuilder.create(calciteConfig);
		RelNode node =
				builder
						.scan(tableName)
						.build();
		node.getRowType().getFieldList().get(0);
		String query = SqlGenerator.getSql(node, config.DIALECT);
		QueryTable resultTable = SqlQueryExecutor.query(query, ConnectionManager.getInstance().getUnioned());
		SecureRelRecordType tableSchema = resultTable.getSchema();

		// step 1: use RexNodes to gather constraints
//		for (RexNode r: customerConstraints){
		for (int i = 0; i < customerConstraints.size(); i++){
			RexNode r = customerConstraints.get(i);
			System.out.println("Accepting rexnode: " + r.toString());

			ConstraintVisitor constraintVisitor = new ConstraintVisitor(tableSchema);
			Comparable c = r.accept(constraintVisitor);
			Map<RexInputRef, ColumnDefinition> columnDefinitionMap = constraintVisitor.getColumnDefinitionMap();
			for (ColumnDefinition columnDefinition: columnDefinitionMap.values()){
				System.out.println("Column Definition");
				System.out.println(columnDefinition);
				System.out.println("Correct Column Definition");
				System.out.println(correctColumnDefinitions.get(i));
				assertTrue(columnDefinition.equals(correctColumnDefinitions.get(i)));
			}
			System.out.println();
		}
	}

	public void testConstraintVisitorColumnDefinitionConstructor() throws Exception {
		String tableName = "customer";
		List<RexNode> customerConstraints = extractTableConstraints("customer");
		// correct column definitions
		// correct column definitions
		List<ColumnDefinition> correctColumnDefinitions = new ArrayList<>(customerConstraints.size());
		// "AND(<=($5, 9999.99:DECIMAL(6, 2)), >=($5, CAST('-999.99'):DECIMAL(19, 0) NOT NULL))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, -999.99, 9999.99, -1L));
		// "AND(<=(1, $0), <=($0, 15000))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, 1, 15000, -1L));
		// "OR(=($6, 'BUILDING'), =($6, 'FURNITURE'), =($6, 'HOUSEHOLD'), =($6, 'MACHINERY'), =($6, 'AUTOMOBILE'))"
		Set<Object> domain = new HashSet<>(Arrays. asList("MACHINERY", "AUTOMOBILE", "BUILDING", "FURNITURE", "HOUSEHOLD"));
		correctColumnDefinitions.add(new ColumnDefinition(null, 5L, -1L, domain, null, null, -1L));
		// "AND(<=(0, $3), <=($3, 24))"
		correctColumnDefinitions.add(new ColumnDefinition(null, -1L, -1L, null, 0, 24, -1L));

		// step 0: use rel builder to get schema
		RelBuilder builder = RelBuilder.create(calciteConfig);
		RelNode node =
				builder
						.scan(tableName)
						.build();
		node.getRowType().getFieldList().get(0);
		String query = SqlGenerator.getSql(node, config.DIALECT);
		QueryTable resultTable = SqlQueryExecutor.query(query, ConnectionManager.getInstance().getUnioned());
		SecureRelRecordType tableSchema = resultTable.getSchema();

		// step 1: use RexNodes to gather constraints
		 Pattern p = Pattern.compile("(?<=\\$)[0-9]+");
		// RexNode r : customerConstraints
		for(int i = 0; i < customerConstraints.size(); i++) {
			RexNode r = customerConstraints.get(i);
			Matcher m = p.matcher(r.toString());
			if (m.find()) {
				String columnString = m.group();
				int columnIndex = Integer.valueOf(columnString);
				System.out.println("Accepting rexnode:" + r.toString());

				SecureRelDataTypeField secureRelDataTypeField = tableSchema.getSecureField(columnIndex);
				ColumnDefinition columnDefinition = ColumnDefinitionFactory.get(secureRelDataTypeField);
				ConstraintVisitor constraintVisitor = new ConstraintVisitor(columnDefinition);
				r.accept(constraintVisitor);
				System.out.println(columnDefinition);
				// assertEquals doesn't work here
				assertTrue(columnDefinition.equals(correctColumnDefinitions.get(i)));
			}
		}
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
