package org.smcql.codegen.smc.operator;

import java.util.*;

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

// TODO: modify sort procedure s.t. if it has a secure child we sort on group-by key and dummy tag
// if it is a secure leaf, then we order by group-by attr and dummy tag in plaintext
// need to revise SQL generator for that
// also need to address sort method to extract dummyTag first

public class SecureGroupByAggregate extends SecureOperator {

	/**
	 *
	 */
	private static final long serialVersionUID = -5374902647734506171L;
	private SecureRelRecordType schema;

	public SecureGroupByAggregate(Operator o) throws Exception {
		super(o);
		schema = o.getSchema();
	}

	@Override
	public Map<String, String> generate() throws Exception {
		Map<String, String> variables = baseVariables();
		Aggregate a = (Aggregate) planNode;


		// TODO: Allow compute to replace operation for the following operations: Count, Sum, Avg, Min, Max
		// Current Priorities are: Count and Sum

		
		List<SecureRelDataTypeField> groupByAttributes = a.getGroupByAttributes();

		String groupByMatch = generateGroupBy(groupByAttributes);
		variables.put("groupByMatch", groupByMatch);

		Map<String, String> scalarVars = getGroupByAggregates();


		for (Map.Entry<String, String> entry : scalarVars.entrySet()) {
			variables.put(entry.getKey(), entry.getValue());
		}


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

		for (SecureRelDataTypeField r : attrs) {
			String lVar = "l" + i;
			String rVar = "r" + i;

			// want lVarInteger(lBits, lhs.bits + offset)

			Pair<Integer, Integer> schemaPos = CodeGenUtils.getSchemaPosition(schema.getAttributes(), r);
			int fieldSize = schemaPos.getKey();
			int fieldOffset = schemaPos.getValue();


			ret += "    Integer " + lVar + "(" + fieldSize + ", lhs.bits + " + fieldOffset + ");\n";
			ret += "    Integer " + rVar + "(" + fieldSize + ", rhs.bits + " + fieldOffset + ");\n";
			ret += "    ret = If(" + lVar + " != " + rVar + ", Bit(0, PUBLIC), Bit(1, PUBLIC));\n";
			ret += "     \n\n";


			++i;
		}

		ret += "    return ret;\n";

		return ret;

	}


	private Map<String, String> getGroupByAggregates() {
		String initAggregate = "";
		String processAggregate = "";
		String writeGroupBy = "";
		String writeAggregate = "";

		int runningOffset = 0;
		int aggCounter = 0;

		Map<String, AggregateCall> aggMap = new HashMap<String, AggregateCall>();


		LogicalAggregate baseAggregate = (LogicalAggregate) planNode.getSecureRelNode().getRelNode();
		
		List<AggregateCall> aggCallList = baseAggregate.getAggCallList();
		Iterator<Pair<AggregateCall, String>> aggItr = baseAggregate.getNamedAggCalls().iterator();

		assert (aggCallList.size() == baseAggregate.getNamedAggCalls().size()); // TODO: add support for anonymous calls

		while (aggItr.hasNext()) {

			Pair<AggregateCall, String> entry = aggItr.next();
			aggMap.put(entry.right, entry.left);
		}

		Iterator<AggregateCall> aggPos = aggCallList.iterator();

		while (aggPos.hasNext()) {
			AggregateCall call = aggPos.next();

			RelDataType field = call.getType();
			int size = TypeMap.getInstance().sizeof(field);
			System.out.println(" Aggregate call for scalar aggregate:" + call);

			int varNo = aggCounter + 1;
			String aggVariable = "agg" + varNo;


			String initAggregateValue = getValueInit(call);

			initAggregate += "Integer " + aggVariable + "= Integer(INT_LENGTH," + initAggregateValue + ",PUBLIC);\n";

			if (!call.getArgList().isEmpty())
				initAggregate += "Integer tupleArg" + varNo + " = Integer(" + size + "," + initAggregateValue + ", PUBLIC);\n";

			processAggregate += getProcessAggregate(call, aggCounter + 1, size, runningOffset);

			writeGroupBy += writeGroupBy("output", aggVariable);

			writeAggregate += writeAggregate(call, aggMap, "output", aggVariable, size);


			aggCounter++;

			runningOffset += size;

		}


		Map<String, String> vars = new HashMap<String, String>();
		vars.put("initAgg", initAggregate);
		vars.put("processAgg", processAggregate);
		vars.put("writeGroupBy", writeGroupBy);
		vars.put("writeAggregate", writeAggregate);

		return vars;


	}



	private String getValueInit(AggregateCall call) {
		// case: min - return 99999
		// case: max, avg, count, sum - return 0


		String varInit = "0";

		switch (call.getAggregation().kind) {

			case MIN:
				varInit = "99999";
				return varInit;
			case MAX:
			case COUNT:
			case SUM:
			case AVG:
				return varInit;
			default:
				return null;
		}


	}

	private String getProcessAggregate(AggregateCall call, int aggId, int size, int runningOffset) {

		String aggVar = "agg" + aggId;
		String tupleVar = "tupleArg" + aggId;
		String processString = "";

		if (!call.getArgList().isEmpty())
			processString = extractAggregateArgument(call, aggVar, tupleVar, size, runningOffset);


		switch (call.getAggregation().kind) {

			case MIN:
				processString += "not yet implemented";
				return processString;
			case MAX:
				processString += "not yet implemented";
				return processString;
			case COUNT:  // TODO: set up for group by part
				processString += "agg" + aggId + " = If(dummyTest, " +  aggVar + " + Integer(INT_LENGTH, 1, PUBLIC), " + aggVar + ");\n";
				return processString;
			case SUM:
				processString += "agg" + aggId + " = If(dummyTest, " + aggVar + " + " + tupleVar + ", " + aggVar + ");\n";
				return processString;
			case AVG:
				processString += "not yet implemented";
				return processString;
			default:
				return null;
		}


	}

	private String extractAggregateArgument(AggregateCall call, String srcVar, String dstVar, int size, int runningOffset) {
		Integer arg = call.getArgList().get(0);
		Integer offset = schema.getFieldOffset(arg);

		return "writeToInteger( &" + dstVar + ", &tuple, 0, " + runningOffset + ", " + size + ");\n";
	}

	private String writeGroupBy(String dstTuple, String aggVar) {
		/* Function to write any GroupBy attributes to output tuples.
		   For example, in the query "SELECT major_icd9, Count(*) FROM diagnoses GROUP BY major_icd9";
		   the major_icd9 attribute would be written by this function.
		   */



		LogicalAggregate agg =  (LogicalAggregate) planNode.getSecureRelNode().getRelNode();
		List<Integer> groupBy;
		groupBy = agg.getGroupSet().asList();
		SecureRelRecordType inSchema = planNode.getInSchema();
		String writer = new String();

		for(Integer i : groupBy) {
			SecureRelDataTypeField field = inSchema.getSecureField(i);
			String name = field.getName();
			int size = TypeMap.getInstance().sizeof(field);
			int srcOffset = inSchema.getFieldOffset(field.getIndex());
			int dstOrdinal = schema.getAttribute(name).getIndex();
			int dstOffset = schema.getFieldOffset(dstOrdinal);
			writer += "writeToInteger(" + dstTuple + ", &" + aggVar + ", " + srcOffset + ", " + dstOffset + ", " + size + ");\n";
		}

		return writer;
	}


	private String writeAggregate(AggregateCall call, Map<String, AggregateCall> aggMap, String dstTuple, String aggVar, int size) {
			/* Function to write any GroupBy aggregates to output tuples.
		   For example, in the query "SELECT major_icd9, Count(*) FROM diagnoses GROUP BY major_icd9";
		   the Count(*) attribute would be written by this function.
		   */

		LogicalAggregate baseAggregate = (LogicalAggregate) planNode.getSecureRelNode().getRelNode();
		RelRecordType record = (RelRecordType) baseAggregate.getRowType();
		
		String writer = new String();

		// Iterate over record
		for (RelDataTypeField field : record.getFieldList()) {
			String name = field.getName();
			int dstOffset = schema.getFieldOffset(field.getIndex());
			//int srcOffset = inSchema.getFieldOffset(inSchema.getFieldOffset(inSchema.getAttribute(name).getIndex()));
			if (aggMap.containsKey(name)) {
				if (aggMap.get(name).equals(call)) {
					writer += "writeToInteger(" + dstTuple + ", &" + aggVar + ", " + dstOffset + ",0, " + size + ");\n";
					break;
				}
			}
		}

		return writer;


	}





	private String getGroupProcessAggregate(AggregateCall call, int aggId, int size, int runningOffset) {

		String aggVar = "agg" + aggId;
		String tupleVar = "tupleArg" + aggId;
		String processString = "";

		if (!call.getArgList().isEmpty())
			processString = extractGroupAggregateArgument(call, aggVar, tupleVar, size, runningOffset);


		switch (call.getAggregation().kind) {

			// TODO: Update for GroupBy

			case MIN:
				processString += "agg" + aggId + " = If(dummyTest, If(" + tupleVar + " < " + aggVar + ", " + tupleVar + ", " + aggVar + "), " + aggVar + ");\n";
				return processString;
			case MAX:
				processString += "agg" + aggId + " = If(dummyTest, If(" + tupleVar + " > " + aggVar + ", " + tupleVar + ", " + aggVar + "), " + aggVar + ");\n";
				return processString;
			case COUNT:
				return "// not yet implemented";
			case SUM:
				processString += "agg" + aggId + " = If(dummyTest," + aggVar + " + " + tupleVar + ", " + aggVar + ");\n";
				return processString;
			case AVG:
				return "not yet implemented";
			default:
				return null;
		}


	}



	private String extractGroupAggregateArgument(AggregateCall call, String srcVar, String dstVar, int size, int runningOffset) {
		Integer arg = call.getArgList().get(0);
		Integer offset = schema.getFieldOffset(arg);

		return "writeToInteger( &" + dstVar + ", &current, 0, " + runningOffset + ", " + size + ");\n";
	}

}
