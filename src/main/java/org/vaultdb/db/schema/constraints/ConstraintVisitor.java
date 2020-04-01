package org.vaultdb.db.schema.constraints;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.GregorianCalendar;
import java.util.List;

import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexCorrelVariable;
import org.apache.calcite.rex.RexDynamicParam;
import org.apache.calcite.rex.RexFieldAccess;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;
import org.apache.calcite.rex.RexLocalRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexOver;
import org.apache.calcite.rex.RexPatternFieldRef;
import org.apache.calcite.rex.RexRangeRef;
import org.apache.calcite.rex.RexSubQuery;
import org.apache.calcite.rex.RexTableInputRef;
import org.apache.calcite.rex.RexVisitor;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Table;
import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;


// adds constraints to SystemCatalog
// TODO: for now constraints only cover a single attribute at a time, extend this
// loosely based on RexInterpreter
public class ConstraintVisitor   implements RexVisitor<Comparable> {

	SystemConfiguration config;
	SecureRelDataTypeField srcField;
	ColumnDefinition<?> constraints;
	SchemaPlus sharedSchema;
	SystemCatalog catalog;
	
	//SecureRelRecordType srcSchema;
	
	
	public ConstraintVisitor(SecureRelDataTypeField field, SecureRelRecordType schema) throws Exception {
		srcField = new SecureRelDataTypeField(field.getBaseField(), field);
		constraints = ColumnDefinitionFactory.get(srcField);
		config = SystemConfiguration.getInstance();
		catalog = SystemCatalog.getInstance();
		sharedSchema = config.getPdfSchema();
	}

	@Override
	public Comparable visitInputRef(RexInputRef inputRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitLocalRef(RexLocalRef localRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitLiteral(RexLiteral literal) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitCall(RexCall call) {
		
		  final List<Comparable> values = new ArrayList<>(call.operands.size());
		    for (RexNode operand : call.operands) {
		      values.add(operand.accept(this));
		    }


		    /*
		    switch (call.getKind()) {
		    	case EQUALS:
		    		return compare(values, c -> c == 0);
		    case IS_DISTINCT_FROM:
		      if (containsNull(values)) {
		        return !values.get(0).equals(values.get(1));
		      }
		      // falls through NOT_EQUALS
		    case NOT_EQUALS:
		      return compare(values, c -> c != 0);
		    case GREATER_THAN:
		      return compare(values, c -> c > 0);
		    case GREATER_THAN_OR_EQUAL:
		      return compare(values, c -> c >= 0);
		    case LESS_THAN:
		      return compare(values, c -> c < 0);
		    case LESS_THAN_OR_EQUAL:
		      return compare(values, c -> c <= 0);
		    case AND:
		      return values.stream().map(Truthy::of).min(Comparator.naturalOrder())
		          .get().toComparable();
		    case OR:
		      return values.stream().map(Truthy::of).max(Comparator.naturalOrder())
		          .get().toComparable();
		    case NOT:
		      return not(values.get(0));
		    case CASE:
		      return case_(values);
		    case IS_TRUE:
		      return values.get(0).equals(true);
		    case IS_NOT_TRUE:
		      return !values.get(0).equals(true);
		    case IS_NULL:
		      return values.get(0).equals(N);
		    case IS_NOT_NULL:
		      return !values.get(0).equals(N);
		    case IS_FALSE:
		      return values.get(0).equals(false);
		    case IS_NOT_FALSE:
		      return !values.get(0).equals(false);
		    case PLUS_PREFIX:
		      return values.get(0);
		    case MINUS_PREFIX:
		      return containsNull(values) ? N
		          : number(values.get(0)).negate();
		    case PLUS:
		      return containsNull(values) ? N
		          : number(values.get(0)).add(number(values.get(1)));
		    case MINUS:
		      return containsNull(values) ? N
		          : number(values.get(0)).subtract(number(values.get(1)));
		    case TIMES:
		      return containsNull(values) ? N
		          : number(values.get(0)).multiply(number(values.get(1)));
		    case DIVIDE:
		      return containsNull(values) ? N
		          : number(values.get(0)).divide(number(values.get(1)));
		    case CAST:
		      return cast(call, values);
		    case COALESCE:
		      return coalesce(call, values);
		    case CEIL:
		    case FLOOR:
		      return ceil(call, values);
		    case EXTRACT:
		      return extract(call, values);
		    default:
		      throw new Exception("call " + call.getKind() + " not yet implemented.");
		    }
		    */
		    
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitOver(RexOver over) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitCorrelVariable(RexCorrelVariable correlVariable) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitDynamicParam(RexDynamicParam dynamicParam) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitRangeRef(RexRangeRef rangeRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitFieldAccess(RexFieldAccess fieldAccess) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitSubQuery(RexSubQuery subQuery) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitTableInputRef(RexTableInputRef fieldRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitPatternFieldRef(RexPatternFieldRef fieldRef) {
		// TODO Auto-generated method stub
		return null;
	}
	
/*
 * 	@Override
 
	public Comparable visitInputRef(RexInputRef inputRef) {
		// should not need this
		
		// verifies that this only references srcField - not supporting composite constraints for now
		if(inputRef.getName().equals(srcField.getName()) && inputRef.getIndex() == srcField.getIndex())
				return true;
		return false;
		
	}

	@Override
	public Comparable visitLocalRef(RexLocalRef localRef) {
		// N/A
		return true;
	}

	@Override
	public Comparable visitLiteral(RexLiteral literal) {
		SqlTypeName type = literal.getTypeName();	 // TODO: may wish to validate against ColumnDefinition type

		final Comparable v = literal.getValue();
/*
	 	// converting to org.apache.calcite.avatica.util.TimeUnitRange
		switch (type) {
         case DATE:
         case TIME:
         case TIMESTAMP:
        	 	final Comparable value = literal.getValue();
        	 	if(value instanceof GregorianCalendar) { // printed datetime
        	 		GregorianCalendar calendar = (GregorianCalendar) value;
        	 		Long timestamp = calendar.getTimeInMillis();
        	 		
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
		 
		
		
		return true;
	}

	@Override
	public Comparable visitCall(RexCall call) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitOver(RexOver over) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitCorrelVariable(RexCorrelVariable correlVariable) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitDynamicParam(RexDynamicParam dynamicParam) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitRangeRef(RexRangeRef rangeRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitFieldAccess(RexFieldAccess fieldAccess) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitSubQuery(RexSubQuery subQuery) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitTableInputRef(RexTableInputRef fieldRef) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Comparable visitPatternFieldRef(RexPatternFieldRef fieldRef) {
		// TODO Auto-generated method stub
		return null;
	}
*/

}
