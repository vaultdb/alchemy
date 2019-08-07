package org.vaultdb.plan.operator;

import java.util.List;

import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.type.SecureRelDataTypeField;

public class CommonTableExpressionScan extends Operator {

	public CommonTableExpressionScan(String name, SecureRelNode src, Operator ...children ) throws Exception {
		super(name, src, children);
		sliceAgnostic = true;
	}
	
	public List<SecureRelDataTypeField> getSliceAttributes() {
		return super.derivedSliceKey();
	}

};
