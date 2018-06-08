package org.smcql.codegen;

import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;

public class CodeCompiler {
	SecureRelRoot queryPlan;
	
	public CodeCompiler(SecureRelRoot root) {
		queryPlan = root;
	}
	
	public void compile(String destination) {
		String queryId = queryPlan.getName();
		Operator root = queryPlan.getPlanRoot();
		
		System.out.println("query: " + queryId);
		traverse(root);
	}
	
	
	//handle pushing/pulling filters and projects
	//handle self join repetition
	
	private void traverse(Operator op) {
		if (op.getExecutionMode() == ExecutionMode.Plain) {
			while (op instanceof Filter || op instanceof Project)
				op = op.getParent();

			String srcSql =  SqlGenerator.getSourceSql(op);
			System.out.println("Source SQL for " + op.getOpName() + ":\n" + srcSql);
			return;
		}
		
		for(Operator child : op.getSources()) {
			if (!child.getSources().isEmpty())
				traverse(child);
			
			if (!(op instanceof Filter || op instanceof Project))
				System.out.println(op.getOpName());
		}
	}
}
