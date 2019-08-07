package org.smcql.codegen.smc.operator.support;

import java.util.GregorianCalendar;

import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;
import org.apache.calcite.rex.RexPatternFieldRef;
import org.apache.calcite.rex.RexTableInputRef;
import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.type.SecureRelRecordType;


public class RexNodeToSmc extends RexFlattener{

	String variableName;  // variable for tuple

	public RexNodeToSmc(SecureRelRecordType aSchema, String aVarName, int srcSize) {
		super(aSchema, srcSize);
		variableName = aVarName;
	}

	@Override 
	public String visitInputRef(RexInputRef inputRef) {
		
		// get type
		SqlTypeName type = inputRef.getType().getSqlTypeName();
		
		// get ordinal
		int ordinal = inputRef.getIndex();
		int startIdx = getStartIdx(ordinal);
		int fieldSize = schema.getSecureField(ordinal).size();
		String variable;
		
		// use FieldFactory pattern to get size, etc.  For now either an integer or float
		if(type == SqlTypeName.DECIMAL || type == SqlTypeName.FLOAT || type == SqlTypeName.DOUBLE) {
			 variable =  "Float("; 
		}
		else { // default setting Integer, should cover strings too
			variable = "Integer(";			
		}
			 
		variable += fieldSize + ", " + ((RexNodeToSmc) this).variableName + ".bits ";
		if(startIdx > 0) {
			variable += " + " + startIdx;
		}
		variable += ")";
		
		return variable;

	}
		private int getStartIdx(int ordinal) {
			int startIdx = 0;
			int fieldIdx = 0;

			while(fieldIdx < ordinal) {
				startIdx += schema.getSecureField(fieldIdx).size();
				++fieldIdx;
			}

			return startIdx;
		}
		

		
		
	/*@Override
	public String visitInputRef(RexInputRef inputRef) {
		return super.visitInputRef(inputRef);

		int idx = inputRef.getIndex();
		
		try {
			return schema.getInputRef(idx, variableName);
		} catch (NullPointerException e) {
			return schema.getInputRef(idx - schema.getFieldCount(), variableName); //occurs when filter comes from a join
		}
		
	}*/

	@Override
	public String visitLiteral(RexLiteral literal) {
		SqlTypeName type = literal.getTypeName();
		

	 	final Comparable v = literal.getValue();

	 	// converting to org.apache.calcite.avatica.util.TimeUnitRange
		switch (type) {
         case DATE:
         case TIME:
         case TIMESTAMP:
        	 	final Comparable value = literal.getValue();
        	 	if(value instanceof GregorianCalendar) { // printed datetime
        	 		GregorianCalendar calendar = (GregorianCalendar) value;
        	 		long timestamp = calendar.getTimeInMillis();
        			return new String("Integer(LENGTH_INT, " + timestamp + ", PUBLIC)" );
        	 	}
        	 return new String("Integer(LENGTH_INT, " + RexLiteral.intValue(literal) + ", PUBLIC)" );
         case FLOAT:
         case DOUBLE:
         case DECIMAL:
				 Float bd = literal.getValueAs(Float.class);
			 	return new String("Float(32, " + bd + ", PUBLIC)");
         case INTEGER:
         case BIGINT:
        	 return new String("Integer(LENGTH_INT, " + RexLiteral.intValue(literal) + ", PUBLIC)" );	 
         case BOOLEAN:
        	 final Comparable boolValue = literal.getValue();

        	 if(boolValue instanceof Boolean) {
            	 String bitValue = "1";
            	 if(((Boolean) boolValue).booleanValue() == false) 
            		 bitValue = "0";
            	 return new String("Bit(" + bitValue + ",  PUBLIC)");
        	 }        	 
         default: // try to convert it to an int
        	System.out.println("Can't convert literal of type " + literal.getValue().getClass() +  " to smc!");
        	System.exit(-1);
        	return new String("Integer(32, " + RexLiteral.intValue(literal) + ", PUBLIC)" );
              
		 }
		 
		
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
