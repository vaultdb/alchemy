package org.smcql.codegen.sql;

import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.adapter.jdbc.JdbcTableScan;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rel.logical.LogicalProject;
import org.apache.calcite.rel.rel2sql.RelToSqlConverter;
import org.apache.calcite.sql.*;
import org.apache.calcite.sql.parser.SqlParserPos;
import org.apache.calcite.tools.FrameworkConfig;
import org.apache.calcite.tools.RelBuilder;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.SeqScan;

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
	
	public static String getDistributedSql(SecureRelRoot root, SqlDialect dialect) throws Exception {
		
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
	

	public static String getStringFromNode(RelNode rel, RelToSqlConverter converter, SqlDialect dialect, boolean filterPullUp) {
		SqlSelect selection = converter.visitChild(0, rel).asSelect();
		// move up filter for union/merge input as needed


		// create list for dummyTags regardless of value ( both true and false will be represented)
		SqlNodeList list = selection.getSelectList();

		// Initialize where clause Node
		SqlNode where = selection.getWhere();

		// logic for existence of dummyTags
		if (where != null && filterPullUp) {

			// if list is empty, then add base SqlNode
			if(list == null) {
				SqlParserPos pos = selection.getParserPosition();
				list = new SqlNodeList(pos);

				SqlNode star = (SqlNode)  SqlIdentifier.star(pos); // check to see what is being inserted
				list.add(star);

			}

			// Add the where clause to the selection criteria
			list.add(selection.getWhere()); //
			selection.setWhere(null);
			selection.setSelectList(list);
		}

		// logic for no dummyTags (all false) with a filter pullup
		else if (filterPullUp){

            if(list == null) {
                SqlParserPos pos = selection.getParserPosition();
                list = new SqlNodeList(pos);

                SqlNode star = (SqlNode)  SqlIdentifier.star(pos); // check to see what is being inserted
                list.add(star);

            }

			SqlLiteral dummy = SqlLiteral.createBoolean(false, list.getParserPosition());
			list.add(dummy);
			selection.setSelectList(list);

		}

		String sqlOut = selection.toSqlString(dialect).getSql();
		sqlOut = sqlOut.replace("\"", "");
		
		
		return sqlOut;
	}
}
