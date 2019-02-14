package org.smcql.codegen.smc.operator.support;

import org.apache.calcite.rex.RexInputRef;
import org.smcql.type.SecureRelRecordType;

public class RexNodeToSmc extends RexFlattener{

	String variableName;  // variable for bitmask

	public RexNodeToSmc(SecureRelRecordType aSchema, String aVarName, int srcSize) {
		super(aSchema, srcSize);
		variableName = aVarName;
	}

	@Override
	public String visitInputRef(RexInputRef inputRef) {
		int idx = inputRef.getIndex();
		
		try {
			return schema.getInputRef(idx, variableName);
		} catch (NullPointerException e) {
			return schema.getInputRef(idx - schema.getFieldCount(), variableName); //occurs when filter comes from a join
		}
		
	}
}
