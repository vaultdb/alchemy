package org.smcql.codegen.operators;

import org.smcql.plan.operator.Operator;

public class MergeNode extends CodeGenNode {

	public MergeNode(Operator operator) {
		super(operator, false);
	}

	public String toString() {
		return "Operators: Merge (" + this.getOperators().get(0).getOpName() + ");";		
	}

}
