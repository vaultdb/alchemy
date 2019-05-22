package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.Map;

import org.apache.calcite.util.Pair;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.WindowAggregate;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.RexNodeUtilities;

// only supports row number for now
public class SecureWindowAggregate extends SecureOperator  {

	/**
	 * 
	 */
	private static final long serialVersionUID = 891090055886639488L;

	public SecureWindowAggregate(Operator o) throws Exception {
		super(o);

	}

	private SecureRelDataTypeField fieldFromSchema(SecureRelRecordType schema, SecureRelDataTypeField field) {
		for (SecureRelDataTypeField f : schema.getSecureFieldList()) {
			if (field.getName().equals(f.getName()))
				return f;
		}
		return field;
	}
	
	@Override
	public Map<String, String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();		
		
		assert(planNode instanceof WindowAggregate);
		
		WindowAggregate win = (WindowAggregate) planNode;
		SecureRelDataTypeField ref = win.getAggregateAttributes().get(0); // only one for row #
		SecureRelRecordType dstSchema = win.getSchema();
		SecureRelDataTypeField windowAttr = win.getSliceAttributes().get(0);  // partition by
		
		// desired outcome:
		//  Integer rowNumSec(INT_SIZE, rowNum, PUBLIC);
		//  memcpy(dstTuple.bits + dstOffset, rowNumSec, INT_SIZE)
		
		String rowNumSec = "Integer rowNumSec(INT_SIZE, rowNum, PUBLIC);\n";

		// field size, field offset in bits
		Pair<Integer, Integer> pos = CodeGenUtils.getSchemaPosition(dstSchema.getAttributes(), ref);
		rowNumSec += "memcpy(dstTuple.bits" + " + " + pos.getValue() + ", rowNumSec, INT_SIZE);\n";
		
		
		variables.put("rowNumAssign", rowNumSec);
		variables.put("mSize", Integer.toString(windowAttr.size()));
		// TODO: fix this one
		variables.put("winMask", dstSchema.getInputRef(windowAttr, "dstTuple"));

		
		// everything else is a 1:1 copy
		if (this.filters.isEmpty()) {
			variables.put("applyFilter", "ret = 1;");
		} else {
			String condition = "";
			String assigns = "";
			int index = 0;
			for (Filter f : this.filters) {
				
				// TODO: need to flatten for mpc with RexNodeUtilities
				// add them up as in flattenFilter
				
				SecureRelRecordType inSchema = f.getChild(0).getSchema(true);
				
				SecureRelDataTypeField field = f.computesOn(inSchema).get(0);
				field = fieldFromSchema(inSchema, field);
				
				String filterCond = RexNodeUtilities.flattenFilter(f, "tuple", Integer.parseInt(variables.get("sSize")));
				condition += (index == 0) ? filterCond : " && " + filterCond;
				
				index++;
			}
			condition = condition.replace("(", "").replace(")", "");
			String filterStr = assigns + "\n\tif (" + condition + ") {\n\t\tret = 1;\n\t}";
			
			variables.put("applyFilter", filterStr);
		}		
		
		String generatedCode = null;
		if(planNode.getExecutionMode() == ExecutionMode.Slice && SystemConfiguration.getInstance().getProperty("sliced-execution").equals("true")) {
			generatedCode = CodeGenUtils.generateFromTemplate("windowAggregate/sliced/row_num.txt", variables);
		}
		else {
			generatedCode = CodeGenUtils.generateFromTemplate("windowAggregate/singular/row_num.txt", variables);
		}
	
		Map<String, String> result = new HashMap<String, String>();
		result.put(getPackageName(), generatedCode);
		return result;
	}
}
