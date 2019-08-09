package org.vaultdb.codegen.smc.operator;

import java.util.*;

import org.apache.calcite.rel.core.AggregateCall;
import org.apache.calcite.rel.logical.LogicalAggregate;
import org.apache.calcite.rel.type.RelDataType;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rel.type.RelRecordType;
import org.vaultdb.plan.operator.Operator;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.apache.calcite.util.Pair;
import org.vaultdb.plan.operator.Aggregate;
import org.vaultdb.type.TypeMap;
import org.vaultdb.util.CodeGenUtils;


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
	public String generate() throws Exception {
		Map<String, String> variables = baseVariables();
		Aggregate a = (Aggregate) planNode;


		List<SecureRelDataTypeField> groupByAttributes = a.getGroupByAttributes();
		variables.put("groupBySize", Integer.toString(getGroupBySize(groupByAttributes)));
		variables.put("initializeGroupBy", "        " + extractGroupByFields(groupByAttributes, "lastGroupBy", "lastTuple"));

		
		
		variables.put("extractGroupBy", extractGroupByFields(groupByAttributes, "groupBy", "tuple"));
		variables.put("writeGroupBy", writeGroupBy(groupByAttributes, "output", "groupBy"));
		
		
		// rolled this into below
		//variables.put("initializeAggregates", initializeAggregates());
		
		Map<String, String> aggregates = generateAggregates();

		// initializeAggregate, computeAggregate, writeOutAggregate
		for (Map.Entry<String, String> entry : aggregates.entrySet()) {
			variables.put(entry.getKey(), entry.getValue());
		}


		
	
		generatedCode = CodeGenUtils.generateFromTemplate("aggregate/groupby/aggregate.txt", variables);

		return generatedCode;
	}

	
	private int getGroupBySize(List<SecureRelDataTypeField> attrs) {
		int size = 0;
		
		for (SecureRelDataTypeField r : attrs) { 
			size += r.size();
		}
		
		return size;
	}
	
	


	
	private Map<String, String> generateAggregates() {
		String initAggregate = "";
		String computeAggregate = "";
		String writeAggregate = "";

		int runningOffset = 0;
		int aggCounter = 0;

		Map<String, AggregateCall> aggMap = new HashMap<String, AggregateCall>();


		LogicalAggregate baseAggregate = (LogicalAggregate) planNode.getSecureRelNode().getRelNode();
		
		List<AggregateCall> aggCallList = baseAggregate.getAggCallList();
		Iterator<Pair<AggregateCall, String>> aggItr = baseAggregate.getNamedAggCalls().iterator();

		assert (aggCallList.size() == baseAggregate.getNamedAggCalls().size());

		while (aggItr.hasNext()) {

			Pair<AggregateCall, String> entry = aggItr.next();
			aggMap.put(entry.right, entry.left);
		}

		Iterator<AggregateCall> aggPos = aggCallList.iterator();

		while (aggPos.hasNext()) {
			aggCounter++;

			AggregateCall call = aggPos.next();

			RelDataType field = call.getType();
			
			int size = TypeMap.getInstance().sizeof(field.getSqlTypeName());


			String aggVariable = "agg" + aggCounter;


			String initAggregateValue = getValueInit(call);

			initAggregate += "Integer " + aggVariable + " = Integer(" + size + "," + initAggregateValue + ",PUBLIC);\n";

			if (!call.getArgList().isEmpty())
				initAggregate += "Integer tupleArg" + aggCounter + " = Integer(" + size + "," + initAggregateValue + ", PUBLIC);\n";

			computeAggregate += getProcessAggregate(call, aggCounter, size, runningOffset);
			writeAggregate += writeAggregate(call, aggMap, "output", aggVariable, size);



			runningOffset += size;

		}
		
		


		Map<String, String> vars = new HashMap<String, String>();
		vars.put("initializeAggregate", initAggregate);
		vars.put("computeAggregate", computeAggregate);
		vars.put("writeAggregate", writeAggregate);

		return vars;


	}



	private String getValueInit(AggregateCall call) {
		// case: min - return 99999
		// case: max, avg, count, sum - return 0



		switch (call.getAggregation().kind) {

			case MIN:
				return "99999";
			case MAX:
				return "0";
			case COUNT:
				return "1"; // first tuple makes our count 1
			case SUM:
			case AVG:
				return "0";
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
			case COUNT:  
				processString += aggVar + " = If(incremental, " +  aggVar + " + Integer(" + size + ", 1, PUBLIC), Integer(" + size + ", 1, PUBLIC));\n";
				return processString;  
			case SUM:
				processString += aggVar + " = If(incremental, " + aggVar + " + " + tupleVar + ", " + aggVar + ");\n";
				return processString;
			case AVG:
				processString += "not yet implemented";
				return processString;
			default:
				return null;
		}


	}

	private String extractAggregateArgument(AggregateCall call, String srcVar, String dstVar, int size, int runningOffset) {

		return "EmpUtilities::writeToInteger( &" + dstVar + ", &tuple, 0, " + runningOffset + ", " + size + ");\n";
	}

	private String writeGroupBy(List<SecureRelDataTypeField> groupByFields, String dstVariable, String srcVariable) {
	      
		String output = new String();
		SecureRelRecordType inSchema = planNode.getInSchema();

		
		for(SecureRelDataTypeField field : groupByFields) {
			int srcOffset = inSchema.getFieldOffset(field.getIndex());
			int size = field.size();
			String name = field.getName();
			int dstOrdinal = schema.getAttribute(name).getIndex();
			
			int dstOffset = schema.getFieldOffset(dstOrdinal);
			if(dstOrdinal > -1)
				output += "EmpUtilities::writeToInteger( &" + dstVariable + ", &" + srcVariable + ", " + dstOffset + ", " + srcOffset + ", " + size + ");\n";
		}

		return output;
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

			if (aggMap.containsKey(name)) {
				if (aggMap.get(name).equals(call)) {
					writer += "EmpUtilities::writeToInteger(&" + dstTuple + ", &" + aggVar + ", " + dstOffset + ",0, " + size + ");\n";
					break;
				}
			}
		}

		return writer;


	}




	private String extractGroupByFields(List<SecureRelDataTypeField> groupByAttributes, String dstVariable, String srcVariable) {

		int size = getGroupBySize(groupByAttributes);
		int dstOffset = 0;
		SecureRelRecordType inSchema = planNode.getInSchema();

		String output =  dstVariable + " = Integer(" + size + ", 0, PUBLIC);\n";
		
		for(SecureRelDataTypeField f : groupByAttributes) {
			int srcOffset = inSchema.getFieldOffset(f.getIndex());

			output += "        EmpUtilities::writeToInteger( &" + dstVariable + ", &" + srcVariable + ", " + dstOffset + ", " + srcOffset + ", " + f.size() + ");\n";
			dstOffset += f.size();
		}

		System.out.println("Extract group by: " + output);
		return output;
	}

}
