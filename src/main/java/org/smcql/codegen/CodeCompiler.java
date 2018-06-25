package org.smcql.codegen;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.codegen.operators.MergeNode;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;
import org.smcql.util.Utilities;

public class CodeCompiler {
	private SecureRelRoot queryPlan;
	private CodeGenNode rootNode;
	
	public CodeCompiler(SecureRelRoot root) {
		queryPlan = root;
		rootNode = null;
	}
	
	public String compile() throws Exception {
		compileSteps(queryPlan.getPlanRoot(), null);
		return this.getEmpCode();
	}
	
	public void writeToDisk(String fileName, String destination) throws Exception {
		if (rootNode == null)
			throw new Exception("Cannot write uncompiled code!");
		
		Utilities.cleanDir(destination);
		Utilities.writeFile(destination + "/" + fileName, getEmpCode());
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
		Operator effectiveParent = parent.getOperators().get(0);
		Operator publicOp = (effectiveParent.isSplittable()) ? effectiveParent : op;
		MergeNode merge = new MergeNode(publicOp);
		parent.addChild(merge);
		parent = merge;
		parent.addChild(new CodeGenNode(publicOp, true));
	}
	
	public String getSourceSQL() throws Exception {
		String result = "";
		result = getSourceSQLRecursive(rootNode, result);
		return result;
	}
	
	private String getSourceSQLRecursive(CodeGenNode node, String str) throws Exception {
		if (node.getChildren().isEmpty())
			str += node.generateSQL() + "\n";
			
		for (CodeGenNode child : node.getChildren()) {
			str = getSourceSQLRecursive(child, str);
		}
		
		return str;
	}
	
	public String getEmpCode() throws Exception {
		return rootNode.generateSMC();
	}
	
	public String getTree() {
		String result = "";
		result = getTreeRecursive(rootNode, 0, result);
		return result.substring(0, result.length()-1);
	}
	
	private String getTreeRecursive(CodeGenNode node, int level, String str) {
		String indent = "";
		for (int i=0; i<level; i++) 
			indent += "\t";
		
		str += indent + node.toString() + "\n";
		
		if (node.getChildren().isEmpty())
			return str;
		
		for (CodeGenNode child : node.getChildren()) 
			str = getTreeRecursive(child, level + 1, str);
					
		return str;
	}
}
