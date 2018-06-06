package org.smcql.codegen;

import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;

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
		if (op.getExecutionMode() == ExecutionMode.Plain) 
			return;
		
		for(Operator child : op.getSources()) {
			if (!child.getSources().isEmpty())
				traverse(child);
			System.out.println(op.getOpName());
		}
	}
}
