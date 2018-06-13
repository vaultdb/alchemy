package org.smcql.codegen.operators;

import java.util.ArrayList;
import java.util.List;

import org.smcql.codegen.emp.EmpGenerator;
import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Project;
import org.smcql.plan.operator.WindowAggregate;

public class CodeGenNode {
	private List<Operator> operators;
	private CodeGenNode parent;
	private List<CodeGenNode> children;
	private boolean isPublic;

	public CodeGenNode(List<Operator> operators, boolean isPublic) {
		this.operators = operators;
		this.isPublic = isPublic;
		this.children = new ArrayList<CodeGenNode>();
	}
	
	public CodeGenNode(Operator operator, boolean isPublic) {
		this.operators = new ArrayList<Operator>();
		this.operators.add(operator);
		this.isPublic = isPublic;
		this.children = new ArrayList<CodeGenNode>();
	}
	
	public String generateSQL() throws Exception {
		if (!isPublic)
			throw new Exception("Cannot generate SQL for non-public node");
		
		Operator op = operators.get(0);
		
		//TODO: check that this doesn't violate security policy
		while (op.getParent() instanceof Project || op.getParent() instanceof Filter) {
			op = op.getParent();
		}
		
		//TODO: make a more permanent fix for src sql generation
		op = (op instanceof WindowAggregate) ? op.getChild(0) : op;
		
		return SqlGenerator.getSourceSql(op);
	}
	
	public String generateSMC() throws Exception {
		if (isPublic)
			throw new Exception("Cannot generate SMC for public node");
			
		return EmpGenerator.getEmpCode(this);
	}
	
	public List<Operator> getOperators() {
		return operators;
	}

	public void addOperator(Operator op) {
		operators.add(op);
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
		child.setParent(this);
		children.add(child);
	}
	
	public boolean isPublic() {
		return isPublic;
	}
	
	public String toString() {
		String result = "Operator(s): ";
		
		for (int i=0; i<operators.size(); i++) {
			result += operators.get(i).getOpName();
			if (i<operators.size()-1)
				result += ", ";
		}
			
		result += "; Execution: " + ((isPublic) ? "public" : "private");
		
		return result;
	}
}
