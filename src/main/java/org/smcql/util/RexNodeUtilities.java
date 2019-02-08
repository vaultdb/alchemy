package org.smcql.util;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalProject;
import org.apache.calcite.rex.RexBuilder;
import org.apache.calcite.rex.RexFieldAccess;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.rex.RexUtil;
import org.apache.calcite.sql.SqlKind;
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
			if(itr.hasNext())
				ret += dstVariable + dstSchema.getBitmask(field) + " = " + flattenForSmc(itr.next(), srcSchema, srcVariable, srcSize) + ";\n";
		}
		
		return ret;
		
	}
	
	public static boolean checkForCardinalityBoundSupportedOps(RexNode r) {
		// TODO:  Check that all predicates are equality checks and that all connectors are conjunctive
		return true;
	}
	
	// TODO: make this more robust by using visitor to check out whole tree
	public static List<SecureRelDataTypeField> getInputRefs(RexNode expr, SecureRelRecordType schema) {
		RexUtil.FieldAccessFinder visitor =  new RexUtil.FieldAccessFinder();
        expr.accept(visitor);
        List<RexFieldAccess> accesses = visitor.getFieldAccessList();
        List<SecureRelDataTypeField> fieldsAccessed = new ArrayList<SecureRelDataTypeField>();
        
        for(RexFieldAccess a : accesses) {
        	// TODO: match this with schema entries
        }
        
        return null;

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
		String result = expr.accept(flatten);
		return result;
	}
	
	
	public static String flattenForSql(RexNode expr, SecureRelRecordType schema) {
		RexFlattener flatten = new RexNodeToSql(schema);
		return expr.accept(flatten);

	}
	
	
}
