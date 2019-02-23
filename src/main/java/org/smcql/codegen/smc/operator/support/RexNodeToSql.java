package org.smcql.codegen.smc.operator.support;

import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexPatternFieldRef;
import org.apache.calcite.rex.RexTableInputRef;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;

public class RexNodeToSql extends RexFlattener {

	public RexNodeToSql(SecureRelRecordType aSchema) {
		super(aSchema, aSchema.size());
	}

	@Override
	public String visitInputRef(RexInputRef inputRef) {
		SecureRelDataTypeField field = schema.getSecureField(inputRef.getIndex());
		String table = field.getStoredTable();
		String attr = field.getStoredAttribute();
		if(table != null && attr != null)
			return table + "." + attr;
		
		return field.getName();
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
