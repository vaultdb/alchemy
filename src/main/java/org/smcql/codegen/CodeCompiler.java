package org.smcql.codegen;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;

public class CodeCompiler {
	private SecureRelRoot queryPlan;
	private CodeGenNode rootNode;
	
	public CodeCompiler(SecureRelRoot root) {
		queryPlan = root;
		rootNode = null;
	}
	
	public void compile(String destination) throws Exception {
		compileSteps(queryPlan.getPlanRoot(), null);
		//emit public and private code to destination
	}
	
	//TODO: handle pushing filters up, inserting merge
	private void compileSteps(Operator op, CodeGenNode parent) throws Exception {	
		if (op.getExecutionMode() == ExecutionMode.Plain) {
			Operator publicOp = (op.getParent().isSplittable()) ? op.getParent() : op;
			parent.addChild(new CodeGenNode(publicOp, true));
			return;
		}
		
		CodeGenNode curNode = new CodeGenNode(op, false);
		if (parent == null) {
			rootNode = curNode;
		} else {
			rootNode.addChild(curNode);
		}			
					
		for(Operator child : op.getSources()) {
			if (!child.getSources().isEmpty())
				compileSteps(child, curNode);			
		}
	}
	
	public void printNodes() {
		printRecursive(rootNode, 0);
	}
	
	private void printRecursive(CodeGenNode node, int level) {
		String indent = "";
		for (int i=0; i<level; i++) 
			indent += "\t";
		
		System.out.println(indent + node.toString());
		
		if (node.getChildren().isEmpty())
			return;
		
		for (CodeGenNode child : node.getChildren()) 
			printRecursive(child, level + 1);
	}
}
