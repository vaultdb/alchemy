package org.vaultdb.codegen.sql;


import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.rel2sql.RelToSqlConverter;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlSelect;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.plan.operator.Operator;


public class SqlGenerator {
	
	protected static SqlDialect postgresDialect = SqlDialect.DatabaseProduct.POSTGRESQL.getDialect();
	
	public static String getSql(RelRoot root, SqlDialect dialect) {
		return getSql(root.rel, dialect);
	}
	
	public static String getSql(RelNode rel, SqlDialect dialect) {	
		RelToSqlConverter converter = new ExtendedRelToSqlConverter(dialect);
		return getStringFromNode(rel, converter, dialect);
	}
	
	public static String getSourceSql(Operator node) {
		SecureRelNode secNode = node.getSecureRelNode();
		RelNode rel = secNode.getRelNode();
		RelToSqlConverter converter = new SecureRelToSqlConverter(postgresDialect, secNode.getPhysicalNode());
		return getStringFromNode(rel, converter, postgresDialect);
	}
	

	
	
	public static String getSourceSql(Operator node, SqlDialect dialect) {
		SecureRelNode secNode = node.getSecureRelNode();
		RelNode rel = secNode.getRelNode();
		RelToSqlConverter converter = new SecureRelToSqlConverter(dialect, secNode.getPhysicalNode());

		

		return getStringFromNode(rel, converter, dialect);
	}
	

	public static String getStringFromNode(RelNode rel, RelToSqlConverter converter, SqlDialect dialect) {
		SqlSelect sql = converter.visitChild(0, rel).asSelect();
		String sqlOut = sql.toSqlString(dialect).getSql();
		sqlOut = sqlOut.replace("\"", "");
		return sqlOut;
		
	}
/*		// move up filter for union/merge input as needed


		// create list for dummyTags regardless of value ( both true and false will be represented)
		SqlNodeList selections = sql.getSelectList();

		SqlNodeList newSelection = selections;		
		SqlNode where = sql.getWhere();
		SqlNode dummyTag = null;
		if(filterPullUp) {
			// if selection list is empty, then add wildcard selection		
			if(selections == null) {
				SqlParserPos pos = sql.getParserPosition();
				SqlNode star = (SqlNode)  SqlIdentifier.star(pos); // check to see what is being inserted
				newSelection.add(star);
	
			}
			
			if(where != null) {
				dummyTag = SqlValidatorUtil.addAlias(where, "dummy_tag");
				sql.setWhere(null);
			}
			else { // where is null, still want dummy tag
				SqlLiteral dummy = SqlLiteral.createBoolean(false, selections.getParserPosition());
				dummyTag = SqlValidatorUtil.addAlias(dummy, "dummy_tag");
			}
			newSelection.add(dummyTag);
			sql.setSelectList(newSelection);
		}
		
		
		
		String sqlOut = sql.toSqlString(dialect).getSql();
		sqlOut = sqlOut.replace("\"", "");
		
		
		return sqlOut;*/
		
	//}
	
	
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
