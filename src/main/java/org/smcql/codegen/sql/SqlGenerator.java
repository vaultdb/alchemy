package org.smcql.codegen.sql;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.rel.rel2sql.RelToSqlConverter;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlIdentifier;
import org.apache.calcite.sql.SqlNode;
import org.apache.calcite.sql.SqlNodeList;
import org.apache.calcite.sql.SqlSelect;
import org.apache.calcite.sql.parser.SqlParserPos;
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
		return sqlMode.equals("debug");
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
		if (selection.getWhere() != null && filterPullUp) {
			SqlNodeList list = selection.getSelectList();

			if(list == null) {
				SqlParserPos pos = selection.getParserPosition();
				list = new SqlNodeList(pos);

				SqlNode star = (SqlNode)  SqlIdentifier.star(pos);
				list.add(star);
				
			}
			list.add(selection.getWhere());
			selection.setWhere(null);
			selection.setSelectList(list);
		}
		
		String sqlOut = selection.toSqlString(dialect).getSql();
		
		sqlOut = sqlOut.replace("\"", "");
		return sqlOut;	
	}
}
