package org.smcql.codegen.smc.operator.support;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexCorrelVariable;
import org.apache.calcite.rex.RexDynamicParam;
import org.apache.calcite.rex.RexFieldAccess;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexLiteral;
import org.apache.calcite.rex.RexLocalRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexOver;
import org.apache.calcite.rex.RexRangeRef;
import org.apache.calcite.rex.RexSubQuery;
import org.apache.calcite.rex.RexVisitor;
import org.apache.calcite.sql.SqlKind;
import org.apache.calcite.sql.type.SqlTypeName;
import org.apache.commons.lang.StringUtils;
import org.smcql.config.SystemConfiguration;
import org.smcql.type.SecureRelRecordType;

// generic flattener for expressions, needs override for input references
public abstract class RexFlattener implements RexVisitor<String> {
	protected SecureRelRecordType schema;
	protected int inputSize;
	
	public RexFlattener(SecureRelRecordType aSchema, int srcSize) {
		schema = aSchema;
		inputSize = srcSize;
	}
	
	
	@Override
	public String visitLocalRef(RexLocalRef localRef) {
		System.out.println("Not yet implemented!");
		System.exit(-1);
		return null;
	}
	
		// SQL version
	@Override
	public String visitLiteral(RexLiteral literal) {
		if (literal.getTypeName().toString().equals("CHAR")) {
			String result = "0";
			for (int i=literal.toString().length()-2; i >0; i--) {
				int val = (int) literal.toString().charAt(i) + 1024;
				result += "," + val;
			}
			return result;
		}
		
		return literal.toString();
	}

	private String getSmcString(RexCall call) {
		SqlKind kind = call.getKind();
		String delimiter;
		Logger logger = null;
		
		try {
			logger = SystemConfiguration.getInstance().getLogger();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		logger.info("Handling call: " + call + " of type " + kind + " with operands " + call.getOperands());
		//handle boolean types
		if (call.getType().getSqlTypeName() == SqlTypeName.BOOLEAN 
				&& call.getOperands().size() >= 2
				&& call.getOperands().get(1) instanceof RexLiteral
				&& !kind.equals(SqlKind.DIVIDE)
				&& !kind.equals(SqlKind.GREATER_THAN_OR_EQUAL)
				&& !kind.equals(SqlKind.LESS_THAN_OR_EQUAL) 
				&& !kind.equals(SqlKind.IS_NULL)) {
			return "(" + ((RexNodeToSmc) this).variableName + ".bits[" + (inputSize - 1) + "])";	// previously  == Bit(1, PUBLIC)
		}
		
		if(kind.equals(SqlKind.AND)) {
			delimiter = "&";
		} else if(kind.equals(SqlKind.OR)) {
			delimiter = "||";
		} else if(kind.equals(SqlKind.CAST)) {// skip these for now
			delimiter = "";
		} else if (kind.equals(SqlKind.LIKE)) {
			return "(" +  ((RexNodeToSmc) this).variableName + ".bits[" + (inputSize - 1) + "] == Bit(1, PUBLIC))";		
		} else if (kind.equals(SqlKind.EQUALS)) {
			delimiter = "==";
		} else if (kind.equals(SqlKind.DIVIDE)) {
			
			// recurse to RexLiteral or RexInputRef
			RexNode lhs = call.getOperands().get(0); // usually RexLiteral or RexInputRef
			RexNode rhs = call.getOperands().get(0);
			
			String lhsStr = lhs.accept(this);
			String rhsStr = rhs.accept(this);
			
			return lhsStr + " / " + rhsStr;
		} else {
			delimiter = call.getOperator().getName();
		}
		List<String> children = new ArrayList<String>();
		
		for(RexNode op : call.operands) {
			//if (op.toString().indexOf("%") > 0)
			//	continue;
			String entry = op.accept(this);
			if (entry.contains(",") && !(op instanceof RexInputRef ) && !(op instanceof RexLiteral)) {
				children = new ArrayList<String>();
				String[] vals = entry.split(",");
				int startIndex = 0;
				for (String v : vals) {
					String offset = "";
					if(startIndex > 0)
						offset = " + " + startIndex;
					String line = "Integer(8, " + ((RexNodeToSmc) this).variableName + ".bits " + offset + ") == Integer(LENGTH_INT, " + v + ", PUBLIC)";
					children.add(line);
					startIndex += 8;
				}
				delimiter = "&";
			} else {
				children.add(entry);
				if (kind.equals(SqlKind.LIKE))
					children.add("one");
			}
			
		}
		
		String separater = (delimiter.equals("=") || delimiter.equals("LIKE")) ? " & " : " " + delimiter + " ";
		String result = StringUtils.join(children, separater);
		return result;
	}
	
	private String getSqlString(RexCall call) {
		String result = "";
		for (int i=0; i<call.operands.size(); i++) {
			RexNode op = call.operands.get(i);
			String entry = (op.toString().contains("$")) ? op.accept(this) : op.toString();
			if (i > 0)
				result += " " + call.getOperator() + " ";
			result += entry;
		}
		return result;
	}
	
	@Override
	public String visitCall(RexCall call) {
		if (this instanceof RexNodeToSmc) {
			return getSmcString(call);
		} else {
			return getSqlString(call);
		}
	}

	@Override
	public String visitOver(RexOver over) {
		return null;
	}

	@Override
	public String visitCorrelVariable(RexCorrelVariable correlVariable) {
		return null;
	}

	@Override
	public String visitDynamicParam(RexDynamicParam dynamicParam) {
		return null;
	}

	@Override
	public String visitRangeRef(RexRangeRef rangeRef) {
		return null;
	}

	@Override
	public String visitFieldAccess(RexFieldAccess fieldAccess) {
		return null;
	}

	@Override
	public String visitSubQuery(RexSubQuery subQuery) {
		return null;
	}

}
