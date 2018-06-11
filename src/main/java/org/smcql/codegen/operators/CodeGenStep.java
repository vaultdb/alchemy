package org.smcql.codegen.operators;

import java.util.ArrayList;
import java.util.List;

import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;

public class CodeGenStep {
	private List<Operator> operators;
	private CodeGenStep parent;
	private List<CodeGenStep> children;
	boolean isPublic;

	public CodeGenStep(List<Operator> operators, boolean isPublic) {
		this.operators = operators;
		this.isPublic = isPublic;
	}
	
	public CodeGenStep(Operator operator, boolean isPublic) {
		this.operators = new ArrayList<Operator>();
		this.operators.add(operator);
		this.isPublic = isPublic;
	}
	
	public String generateSQL() throws Exception {
		if (!isPublic)
			throw new Exception("Cannot generate SQL for non-public step");
		
		for (Operator op : operators) {
			Operator cur = op;
			while (cur instanceof Project) {
				cur = op.getChild(0);
			}
			
			return SqlGenerator.getSourceSql(cur);
		}
		
		throw new Exception("Failed to generate SQL for step");
	}
	
	public List<Operator> getOperators() {
		return operators;
	}

	public CodeGenStep getParent() {
		return parent;
	}

	public void setParent(CodeGenStep parent) {
		this.parent = parent;
	}

	public List<CodeGenStep> getChildren() {
		return children;
	}

	public void addChild(CodeGenStep child) {
		if (children == null) 
			children = new ArrayList<CodeGenStep>();
		
		children.add(child);
	}
}
