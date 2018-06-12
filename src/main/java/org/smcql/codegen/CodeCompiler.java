package org.smcql.codegen;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.codegen.operators.MergeNode;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;

public class CodeCompiler {
	private SecureRelRoot queryPlan;
	private CodeGenNode rootNode;
	
	public CodeCompiler(SecureRelRoot root) {
		queryPlan = root;
		rootNode = null;
	}
	
	public void compile(String destination) throws Exception {
		compileSteps(queryPlan.getPlanRoot(), null);
		//TODO: emit public and private code to destination
	}
	
	private void compileSteps(Operator op, CodeGenNode parent) throws Exception {	
		if (op.getExecutionMode() == ExecutionMode.Plain) {
			insertPublicNode(op, parent);
			return;
		}
		
		CodeGenNode curNode = new CodeGenNode(op, false);
		if (parent == null) {
			rootNode = curNode;
		} else {
			parent.addChild(curNode);
		}			
					
		for(Operator child : op.getSources()) {
			child = fuseOperator(child, curNode);
			compileSteps(child, curNode);			
		}
	}
	
	private Operator fuseOperator(Operator op, CodeGenNode node) throws Exception {
		Operator cur = op;
		while (cur instanceof Filter || cur instanceof Project) {
			node.addOperator(cur);
			
			if (cur.getChildren().size() > 1)
				throw new Exception("Cannot fuse operators with multiple children");
			
			cur = cur.getChild(0);
		}
		return cur;
	}
	
	
	private void insertPublicNode(Operator op, CodeGenNode parent) {
		Operator publicOp = (op.getParent().isSplittable()) ? op.getParent() : op;
		MergeNode merge = new MergeNode(publicOp);
		parent.addChild(merge);
		parent = merge;
		parent.addChild(new CodeGenNode(publicOp, true));
	}
	
	public void printSourceSQL() throws Exception {
		printSourceSQLRecursive(rootNode);
	}
	
	private void printSourceSQLRecursive(CodeGenNode node) throws Exception {
		if (node.getChildren().isEmpty())
			System.out.println(node.generateSQL());
			
		for (CodeGenNode child : node.getChildren()) {
			printSourceSQLRecursive(child);
		}
	}
	
	public void printEmpCode() throws Exception {
		System.out.println(rootNode.generateSMC());
	}
	
	public void printNodes() {
		printNodesRecursive(rootNode, 0);
	}
	
	private void printNodesRecursive(CodeGenNode node, int level) {
		String indent = "";
		for (int i=0; i<level; i++) 
			indent += "\t";
		
		System.out.println(indent + node.toString());
		
		if (node.getChildren().isEmpty())
			return;
		
		for (CodeGenNode child : node.getChildren()) 
			printNodesRecursive(child, level + 1);
	}
}
