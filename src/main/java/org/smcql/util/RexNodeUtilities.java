package org.smcql.util;

import java.util.Iterator;
import java.util.List;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalProject;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.util.Pair;
import org.smcql.codegen.smc.operator.support.RexFlattener;
import org.smcql.codegen.smc.operator.support.RexNodeToSmc;
import org.smcql.codegen.smc.operator.support.RexNodeToSql;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Project;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;

public class RexNodeUtilities {
	
	public static String flattenProjection(Project aProject, String srcVariable, String dstVariable, int srcSize) {
		SecureRelRecordType srcSchema = aProject.getInSchema();
		SecureRelRecordType dstSchema = aProject.getSchema();
		String ret = new String();
		LogicalProject projection = (LogicalProject) aProject.getSecureRelNode().getRelNode();
		
		assert(projection.getChildExps().size() == dstSchema.getFieldCount());
		List<RexNode> fieldProject = projection.getChildExps();
		Iterator<RexNode> itr = fieldProject.iterator();

		for(SecureRelDataTypeField field : dstSchema.getSecureFieldList()) {
			if(itr.hasNext()) {
				//ret += dstVariable + dstSchema.getInputRef(field, null) + " = " + flattenForSmc(itr.next(), srcSchema, srcVariable, srcSize) + ";\n";
				Pair<Integer, Integer> schemaPos = CodeGenUtils.getSchemaPosition(dstSchema.getAttributes(),field);
				int dstOffset = schemaPos.getValue();
				int dstSize = schemaPos.getKey();
				
				ret += "memcpy(" + dstVariable + ".bits ";
				if(dstOffset > 0) {
					ret += " + " + dstOffset;
				}
				ret += ", " + flattenForSmc(itr.next(), srcSchema, srcVariable, srcSize) + ".bits";
				ret += ", " + dstSize + ");\n";
			}
		}
		
		return ret;
		
	}
	
	
	
	
	
	
	public static String flattenFilter(Filter aFilter, String srcVariable, int srcSize) {
		SecureRelRecordType schema = aFilter.getChild(0).getSchema(true);
		String ret = new String();
		LogicalFilter filter = (LogicalFilter) aFilter.getSecureRelNode().getRelNode();
		
		List<RexNode> fieldFilter = filter.getChildExps();
		Iterator<RexNode> itr = fieldFilter.iterator();
		
		while (itr.hasNext()) {
			ret += flattenForSmc(itr.next(), schema, srcVariable, srcSize);
		}

		return ret;
	}
	
	
	public static String flattenForSmc(RexNode expr, SecureRelRecordType schema, String variable, int srcSize) {
		RexFlattener flatten = new RexNodeToSmc(schema, variable, srcSize);
		//System.out.println("Flattening expression: " + expr + " from schema " + schema + " for variable " + variable);
		String result = expr.accept(flatten);
		return result;
	}
	
	
	public static String flattenForSql(RexNode expr, SecureRelRecordType schema) {
		RexFlattener flatten = new RexNodeToSql(schema);
		return expr.accept(flatten);

	}
	
	
}
