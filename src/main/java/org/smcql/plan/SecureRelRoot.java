package org.smcql.plan;

import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.sql.SqlExplainFormat;
import org.apache.calcite.sql.SqlExplainLevel;
import org.smcql.config.SystemConfiguration;
import org.smcql.parser.SqlStatementParser;
import org.smcql.parser.TreeBuilder;
import org.smcql.plan.operator.Operator;
import java.util.logging.Logger;

public class SecureRelRoot {
	RelRoot baseRoot;
	Operator treeRoot;
	String name;
	
	public SecureRelRoot(String queryName, String sql) throws Exception {
		SqlStatementParser parser = new SqlStatementParser();


		baseRoot = parser.convertSqlToRelMinFields(sql);

		Logger log = SystemConfiguration.getInstance().getLogger();

		//this is just for logging
		log.info("SecureRelRoot.java :  Post Optimization : "+ RelOptUtil.dumpPlan("", baseRoot.rel, SqlExplainFormat.TEXT, SqlExplainLevel.ALL_ATTRIBUTES));


		baseRoot = parser.optimize(baseRoot); // optimized to represent in a fine granularity for more smc avoidance


		//this is just for logging

		log.info("SecureRelRoot.java :  Post Optimization : "+ RelOptUtil.dumpPlan("", baseRoot.rel, SqlExplainFormat.TEXT, SqlExplainLevel.ALL_ATTRIBUTES));

		baseRoot = parser.trimFields(baseRoot); // use minimal set of fields to avoid triggering unnecessary SMC

		
		baseRoot = parser.mergeProjects(baseRoot); // drop any unnecessary steps in the plan

		name = (queryName == null) ? SystemConfiguration.getInstance().getQueryName() : queryName.replaceAll("-", "_");
		treeRoot = TreeBuilder.create(name, baseRoot);
	}
	
	
	public RelRoot getRelRoot() {
		return baseRoot;
	}
	
	public Operator getPlanRoot() {
		return treeRoot;
	}
	
	public String toString() {
		return appendOperator(treeRoot, new String(), "");
	}
	
	public String getName() {
		return name;
	}
	
	String appendOperator(Operator op, String src, String linePrefix) {
		src += linePrefix + op + "\n";
		linePrefix += "    ";
		for(Operator child : op.getChildren()) {
			src = appendOperator(child, src, linePrefix);
		}
		return src;
	}
	
}
