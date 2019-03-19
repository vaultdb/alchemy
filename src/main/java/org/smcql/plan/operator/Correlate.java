package org.smcql.plan.operator;

import org.smcql.plan.SecureRelNode;

public class Correlate extends Operator {

	// placeholder for parsing correlate
	// temp to make it possible for optimizer to rewrite expression into a join
	public Correlate(String name, SecureRelNode src, Operator[] childOps) throws Exception {
		super(name, src, childOps);
		// TODO Auto-generated constructor stub
	}

}
