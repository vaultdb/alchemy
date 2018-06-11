package org.smcql.codegen.operators;

import java.util.ArrayList;
import java.util.List;

import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;

public class CodeGenNode {
	private List<Operator> operators;
	private CodeGenNode parent;
	private List<CodeGenNode> children;
	private boolean isPublic;

	public CodeGenNode(List<Operator> operators, boolean isPublic) {
		this.operators = operators;
		this.isPublic = isPublic;
	}
	
	public CodeGenNode(Operator operator, boolean isPublic) {
		this.operators = new ArrayList<Operator>();
		this.operators.add(operator);
		this.isPublic = isPublic;
	}
	
	public String generateSQL() throws Exception {
		if (!isPublic)
			throw new Exception("Cannot generate SQL for non-public node");
		
		for (Operator op : operators) {
			Operator cur = op;
			while (cur instanceof Project) {
				cur = op.getChild(0);
			}
			
			return SqlGenerator.getSourceSql(cur);
		}
		
		throw new Exception("Failed to generate SQL for step");
	}
	
	public String generateSMC() throws Exception {
		if (isPublic)
			throw new Exception("Cannot generate SMC for public node");
			
		return "";
	}
	
	public List<Operator> getOperators() {
		return operators;
	}

	public CodeGenNode getParent() {
		return parent;
	}

	public void setParent(CodeGenNode parent) {
		this.parent = parent;
	}

	public List<CodeGenNode> getChildren() {
		return children;
	}

	public void addChild(CodeGenNode child) {
		if (children == null) 
			children = new ArrayList<CodeGenNode>();
		
		child.setParent(this);
		children.add(child);
	}
	
	public boolean isPublic() {
		return isPublic;
	}
}
