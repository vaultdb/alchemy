package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.apache.calcite.util.Pair;
import org.smcql.plan.operator.Aggregate;
import org.smcql.util.CodeGenUtils;

public class SecureAggregate extends SecureOperator {

	/**
	 * 
	 */
	private static final long serialVersionUID = -5374902647734506171L;
	private SecureRelRecordType schema;
	public SecureAggregate(Operator o) throws Exception {
		super(o);
		schema = o.getSchema();
	}
	
	@Override
	public Map<String, String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();	
		Aggregate a = (Aggregate) planNode;

		// TODO: Allow compute to replace operation for the following operations: Count, Sum, Avg, Min, Max
		// Current Priorities are: Count and Sum

		String compute = "";
		compute += "secure int$size deref = merged[mIdx];\n";
		compute += "secure int$size toAdd = a[aIdx];\n";
		compute += "deref$cntMask = deref$cntMask + toAdd$cntMask;\n";
		compute += "merged[mIdx] = deref;\n";
		variables.put("compute", compute);
		
		// TODO: add more aggregates; currently hardcoded for groupby aggregate (full oblivious)
		List<SecureRelDataTypeField> groupByAttributes = a.getGroupByAttributes();
		if (groupByAttributes.isEmpty()) {
			Map<String, String> result = new HashMap<String, String>();

			result.put(getPackageName(), CodeGenUtils.generateFromTemplate("aggregate/groupby/aggregate.txt", variables));
			return result;
		} 
		
		String groupByMatch = generateGroupBy(groupByAttributes);
		variables.put("groupByMatch", groupByMatch);
			
		int aggregateIdx = a.getComputeAttributeIndex();
		String cntMask = planNode.getSchema().getInputRef(aggregateIdx, null);
		
		variables.put("cntMask", cntMask);
	
		generatedCode = CodeGenUtils.generateFromTemplate("aggregate/groupby/aggregate.txt", variables);
		
		Map<String, String> result = new HashMap<String, String>();
		result.put(getPackageName(), generatedCode);
		return result;
	}
	
	// takes in a list of attrs we are grouping by
	private String generateGroupBy(List<SecureRelDataTypeField> attrs) throws Exception {
		int i = 1;
		String ret = "    Bit ret(1, PUBLIC);\n\n";
		
		for(SecureRelDataTypeField r : attrs) {
			int size = r.size();
			String lVar = "l" + i;
			String rVar = "r" + i;
			
			// want lVarInteger(lBits, lhs.bits + offset)
			
			Pair<Integer, Integer> schemaPos = CodeGenUtils.getSchemaPosition(schema.getAttributes(), r);
			int fieldSize = schemaPos.getKey();
			int fieldOffset = schemaPos.getValue();
			
			
			ret += "    Integer " +  lVar + "(" + fieldSize + ", lhs.bits + " + fieldOffset + ");\n";
			ret += "    Integer " +  rVar + "(" + fieldSize + ", rhs.bits + " + fieldOffset + ");\n";
			ret += "    ret = If(" + lVar + " != " + rVar + ", Bit(0, PUBLIC), Bit(1, PUBLIC));\n";
			ret += "     \n\n";
			
			
			++i;
		}
		
		ret += "    return ret;\n";
		
		return ret;
		
	}
	

}
