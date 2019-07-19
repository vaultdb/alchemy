package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.calcite.rel.core.AggregateCall;
import org.apache.calcite.rel.logical.LogicalAggregate;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rel.type.RelRecordType;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.apache.calcite.util.Pair;
import org.smcql.plan.operator.Aggregate;
import org.smcql.type.TypeMap;
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
			Map<String, String> scalarVars = getScalarAggregates();


			for(Map.Entry<String, String> entry : scalarVars.entrySet()) {
				variables.put(entry.getKey(), entry.getValue());
			}

			result.put(getPackageName(), CodeGenUtils.generateFromTemplate("aggregate/scalar/aggregate.txt", variables));
			return result;
		}


		int aggregateIdx = a.getComputeAttributeIndex();
		String cntMask = planNode.getSchema().getInputRef(aggregateIdx, null);

		variables.put("cntMask", cntMask);

		generatedCode = CodeGenUtils.generateFromTemplate("aggregate/groupby/aggregate.txt", variables);

		Map<String, String> result = new HashMap<String, String>();
		result.put(getPackageName(), generatedCode);
		return result;
	}
	// variable --> value
	private Map<String, String> getScalarAggregates(){

		String initAggregate = "";
		String processAggregate = "";
		String writeAggregate = "";
		int runningOffset = 0;
		int aggCounter = 0;

		Map<String, AggregateCall> aggMap = new HashMap<String, AggregateCall>();



		LogicalAggregate baseAggregate = (LogicalAggregate) planNode.getSecureRelNode().getRelNode();

		List<AggregateCall> aggCallList = baseAggregate.getAggCallList();
		Iterator<Pair<AggregateCall, String> > aggItr = baseAggregate.getNamedAggCalls().iterator();

		assert(aggCallList.size() == baseAggregate.getNamedAggCalls().size()); // TODO: add support for anonymous calls

		while(aggItr.hasNext()) {

			Pair<AggregateCall, String> entry = aggItr.next();
			aggMap.put(entry.right, entry.left);
		}

		Iterator<AggregateCall> aggPos = aggCallList.iterator();

		while(aggPos.hasNext()) {
			AggregateCall call = aggPos.next();

			RelDataType field = call.getType();
			int size = TypeMap.getInstance().sizeof(field);
			System.out.println(" Aggregate call for scalar aggregate:" + call);

			int varNo = aggCounter + 1;
			String aggVariable = "agg" + varNo;



			String initAggregateValue = getValueInit(call);

			initAggregate += "Integer " + aggVariable + "= Integer(INT_LENGTH," + initAggregateValue + ",PUBLIC);\n";

			if(!call.getArgList().isEmpty())
				initAggregate += "Integer tupleArg" + varNo + " = Integer(" + size  + "," + initAggregateValue + ", PUBLIC);\n";

			processAggregate += getProcessAggregate(call,aggCounter+1,size, runningOffset);

			writeAggregate += getWriteDest(call, aggMap, "output", aggVariable, size);

			aggCounter++;

			runningOffset += size;

		}


		Map<String, String> vars = new HashMap<String, String>();
		vars.put("initAgg", initAggregate);
		vars.put("processAgg",processAggregate);
		vars.put("writeAgg",writeAggregate);

		return vars;
	}


	private String getValueInit(AggregateCall call){
		// case: min - return 99999
		// case: max, avg, count, sum - return 0


		String varInit = "0";

		switch(call.getAggregation().kind) {

			case MIN:
				varInit = "99999";
				return varInit;
			case MAX:
				return varInit;
			case COUNT:
				return varInit;
			case SUM:
				return varInit;
			case AVG:
				return varInit;
			default:
				return null;
		}


	}

	private String getProcessAggregate(AggregateCall call, int aggId, int size, int runningOffset){

		String aggVar = "agg" + aggId;
		String tupleVar = "tupleArg" + aggId;
		String processString = "";

		if(!call.getArgList().isEmpty())
			processString = extractAggregateArgument(call, aggVar, tupleVar, size, runningOffset);



		switch(call.getAggregation().kind) {

			case MIN:
				processString += "agg" + aggId + " = If(dummyCheck, If(" + tupleVar + " < " + aggVar + ", " + tupleVar + ", " + aggVar + "), " + aggVar + ");\n";
				return processString;
			case MAX:
				processString += "agg" + aggId + " = If(dummyCheck, If(" + tupleVar + " > " + aggVar + ", " + tupleVar + ", " + aggVar + "), " + aggVar + ");\n";
				return processString;
			case COUNT:
				processString += "agg" + aggId + " = If(dummyCheck, " +  aggVar + " + Integer(INT_LENGTH, 1, PUBLIC), " + aggVar + ");\n";
				return processString;
			case SUM:
				processString += "agg" + aggId + " = If(dummyCheck," + aggVar + " + " + tupleVar + ", " + aggVar + ");\n";
				return processString;
			case AVG:
				return "not yet implemented";
			default:
				return null;
		}


	}

	private String extractAggregateArgument(AggregateCall call, String srcVar, String dstVar, int size, int runningOffset) {
		Integer arg = call.getArgList().get(0);

		return "EmpUtilities::writeToInteger( &" + dstVar + ", &tuple, 0, " + runningOffset + ", " + size  + ");\n";
	}

	private String getWriteDest(AggregateCall call, Map<String, AggregateCall> aggMap, String dstTuple, String aggVar, int size){

		LogicalAggregate baseAggregate = (LogicalAggregate) planNode.getSecureRelNode().getRelNode();
		RelRecordType record = (RelRecordType) baseAggregate.getRowType();

		String writer = new String();

		// Iterate over record
		for(RelDataTypeField field : record.getFieldList()) {
			String name = field.getName();
			int offset = schema.getFieldOffset(field.getIndex());

			if(aggMap.containsKey(name)) {
				if(aggMap.get(name).equals(call)) {
					writer += "EmpUtilities::writeToInteger(" + dstTuple + ", &" + aggVar + ", " + offset  + ", 0, " + size  + ");\n";
					break;
				}
			}
		}

		return writer;


	}



}