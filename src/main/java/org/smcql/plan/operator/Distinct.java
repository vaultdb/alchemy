package org.smcql.plan.operator;

import java.util.List;

import org.smcql.plan.SecureRelNode;
import org.smcql.type.SecureRelDataTypeField;

public class Distinct extends Operator {

	public Distinct(String name, SecureRelNode src, Operator ...children ) throws Exception {
		super(name, src, children);
		splittable = true;
		blocking = true;
		sliceAgnostic = true;

		
	}
	
	public List<SecureRelDataTypeField> getSliceAttributes() {
		return super.derivedSliceKey();
	}
	
	
	public List<SecureRelDataTypeField> computesOn() {
		return getSchema().getSecureFieldList();
	}

	public List<SecureRelDataTypeField> secureComputeOrder() {
		return getSchema().getSecureFieldList();
	}

	
	@Override
	public int getPerformanceCost(int n) {
		// n*log(n)^2
		return n*(int)Math.pow(Math.log((double)n),2);
	}
	
	@Override
	public void initializeStatistics() {
		// TODO: implement this, might fall into the Operator generic case depending on presence of projection
		children.get(0).initializeStatistics();

	}
	
	
};
