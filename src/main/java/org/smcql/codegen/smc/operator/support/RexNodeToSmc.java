package org.smcql.codegen.smc.operator.support;

import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;
import org.apache.calcite.rex.RexPatternFieldRef;
import org.apache.calcite.rex.RexTableInputRef;
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
	@Override
	public String visitLiteral(RexLiteral literal) {
		// only handles ints for now
		return new String("Integer(LENGTH_INT, " + RexLiteral.intValue(literal) + ", PUBLIC)" );
	}

	@Override
	public String visitTableInputRef(RexTableInputRef fieldRef) {
		// TODO: verify this
		return fieldRef.toString();
	}

	@Override
	public String visitPatternFieldRef(RexPatternFieldRef fieldRef) {
		// TODO: verify this
		return fieldRef.toString();
	}
}
