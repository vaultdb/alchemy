package org.vaultdb.plan.operator;

import java.util.List;

import org.apache.calcite.rel.logical.LogicalAggregate;
import org.apache.calcite.util.ImmutableBitSet;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.type.SecureRelDataTypeField;

import com.google.common.collect.ImmutableList;

public class Distinct extends Operator {

	public Distinct(String name, SecureRelNode src, Operator ...children ) throws Exception {
		super(name, src, children);
		splittable = false;
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

		
		return getSchema().getSecureFieldList(); // all inputs to distinct are in order-by
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
