package org.smcql.codegen;

import org.smcql.codegen.operators.CodeGenStep;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;

public class CodeCompiler {
	private SecureRelRoot queryPlan;
	private CodeGenStep rootStep;
	
	public CodeCompiler(SecureRelRoot root) {
		queryPlan = root;
		
	}
	
	public void compile(String destination) throws Exception {
		String queryId = queryPlan.getName();
		Operator root = queryPlan.getPlanRoot();
		
		System.out.println("query: " + queryId);
		traverse(root);
	}
	
	
	//TODO: handle pushing filters up, inserting merge
	private void traverse(Operator op) throws Exception {
		if (op.getExecutionMode() == ExecutionMode.Plain) {
			Operator publicOp = (op.getParent().isSplittable()) ? op.getParent() : op;
			addStep(new CodeGenStep(publicOp, true));			
			return;
		}
		
		for(Operator child : op.getSources()) {
			if (!child.getSources().isEmpty())
				traverse(child);
			
			System.out.println(op.getOpName());
		}
	}
		
	private void addStep(CodeGenStep step) {
		if (rootStep == null) {
			rootStep = step;
		} else {
			rootStep.addChild(step);
		}
	}
}
