package org.smcql.codegen.sql;


import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.rel2sql.RelToSqlConverter;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlIdentifier;
import org.apache.calcite.sql.SqlLiteral;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.SqlSelect;
import org.apache.calcite.sql.parser.SqlParserPos;
//import org.apache.calcite.sql.ddl.SqlAttributeDefinition;
import org.apache.calcite.sql.validate.SqlValidatorUtil;

//import org.apache.calcite.sql.ddl.SqlAttributeDefinition;


import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.operator.Operator;


public class SqlGenerator {
	
	protected static SqlDialect postgresDialect = SqlDialect.DatabaseProduct.POSTGRESQL.getDialect();
	
	public static String getSql(RelRoot root, SqlDialect dialect) {
		return getSql(root.rel, dialect);
	}
	
	public static String getSql(RelNode rel, SqlDialect dialect) {	
		RelToSqlConverter converter = new ExtendedRelToSqlConverter(dialect);
		return getStringFromNode(rel, converter, dialect, filterPullUpConfig());
	}
	
	public static String getSourceSql(Operator node) {
		SecureRelNode secNode = node.getSecureRelNode();
		RelNode rel = secNode.getRelNode();
		RelToSqlConverter converter = new SecureRelToSqlConverter(postgresDialect, secNode.getPhysicalNode());
		boolean filterPullUp = filterPullUpConfig() & node.pullUpFilter();
		return getStringFromNode(rel, converter, postgresDialect, filterPullUp);
	}
	
	// debug mode = no filter pull-up
	private static boolean filterPullUpConfig() {
		String sqlMode = "release";
		try {
			sqlMode = SystemConfiguration.getInstance().getProperty("code-generator-mode");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return sqlMode.equals("release");
	}
	public static String getSourceSql(Operator node, SqlDialect dialect) {
		SecureRelNode secNode = node.getSecureRelNode();
		RelNode rel = secNode.getRelNode();
		RelToSqlConverter converter = new SecureRelToSqlConverter(dialect, secNode.getPhysicalNode());

		boolean filterPullUp = filterPullUpConfig() || node.pullUpFilter();

		return getStringFromNode(rel, converter, dialect, filterPullUp);
	}
	

	public static String getStringFromNode(RelNode rel, RelToSqlConverter converter, SqlDialect dialect, boolean filterPullUp) {
		SqlSelect selection = converter.visitChild(0, rel).asSelect();
		// move up filter for union/merge input as needed


		// create list for dummyTags regardless of value ( both true and false will be represented)
		SqlNodeList selections = selection.getSelectList();

		SqlNodeList newSelection = selections;		
		SqlNode where = selection.getWhere();
		
		if(where != null && filterPullUp) {
			SqlNode dummyTag = SqlValidatorUtil.addAlias(where, "dummy_tag");
			newSelection = SqlNodeList.of(dummyTag);			
			selection.setWhere(null);

		}
		else if(filterPullUp) { // where is null, still want dummy tag
			SqlLiteral dummy = SqlLiteral.createBoolean(false, selections.getParserPosition());
			SqlNode dummyTag = SqlValidatorUtil.addAlias(dummy, "dummy_tag");
			newSelection = SqlNodeList.of(dummyTag);			
		}
		
		
		// if list is empty, then add wildcard selection
		if(filterPullUp) {
			if(selections == null) {
				SqlParserPos pos = selection.getParserPosition();
				SqlNode star = (SqlNode)  SqlIdentifier.star(pos); // check to see what is being inserted
				newSelection.add(star);
	
			}
			else {
					for(SqlNode node : selections) {
						newSelection.add(node);
					}
				}
			selection.setSelectList(newSelection);
		} // otherwise leave selection alone
		
		
		String sqlOut = selection.toSqlString(dialect).getSql();
		sqlOut = sqlOut.replace("\"", "");
		
		
		return sqlOut;
	}
	
	
	/*	public static String getDistributedSql(SecureRelRoot root, SqlDialect dialect) throws Exception {
	
	Operator rootOp = root.getPlanRoot();
	RelNode distributed = rewriteForDistributed(rootOp);
	RelToSqlConverter converter = new ExtendedRelToSqlConverter(dialect);	
	return getStringFromNode(distributed, converter, dialect, false);
	

	
}

// new tactic: create a new tree of relnodes with the use of copy for everything except the leafs
private static RelNode rewriteForDistributed(Operator op) throws Exception {
	List<RelNode> newChildren = new ArrayList<RelNode>();
	RelNode node = op.getSecureRelNode().getRelNode();
	
	// if it has children, recurse to them first
	if(!op.getChildren().isEmpty()) {
		for(Operator childOp : op.getChildren()) {
			newChildren.add(rewriteForDistributed(childOp));			
		}
	
		return node.copy(node.getTraitSet(), newChildren);
	}

	// else if it is a leaf
	assert(op instanceof SeqScan);
	FrameworkConfig config = SystemConfiguration.getInstance().getCalciteConfiguration();
	RelBuilder relBuilder = RelBuilder.create(config);
	JdbcTableScan scan =  (JdbcTableScan) node;
	String tableName = scan.getTable().getQualifiedName().get(0);

	String remoteTableName = "remote_" + tableName;
	return relBuilder.scan(tableName).scan(remoteTableName).union(true).build();
	
}

*/
}
