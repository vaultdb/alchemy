package org.vaultdb.plan.operator;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.calcite.rel.logical.LogicalProject;
import org.apache.calcite.rex.RexInputRef;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.sql.SqlKind;
import org.vaultdb.plan.SecureRelNode;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;

public class Project extends Operator {
	
	public Project(String name, SecureRelNode src, Operator[] childOps) throws Exception {
		super(name, src, childOps);
		sliceAgnostic = true;
	}
	
	public List<SecureRelDataTypeField> getSliceAttributes() {
		LogicalProject project = (LogicalProject) baseRelNode.getRelNode();
		List<RexNode> projExpressions = project.getChildExps();
		
		List<SecureRelDataTypeField> baseKey = children.get(0).getSliceAttributes();
		List<SecureRelDataTypeField> derivedKey = new ArrayList<SecureRelDataTypeField>();
		SecureRelRecordType projSchema = getSchema();
		
		// iterate over source slice attributes
		for(SecureRelDataTypeField f : baseKey) {
			int dstIdx = 0;
			
			// for each projection expression
			for(RexNode rex : projExpressions) {

				if(rex.getKind() == SqlKind.INPUT_REF) {
					int ordinal = ((RexInputRef) rex).getIndex();
					
					if(ordinal == f.getBaseField().getIndex()) {
						derivedKey.add(projSchema.getSecureField(dstIdx));
						break;
					}
				}
				++dstIdx;
			}
		}
		return derivedKey;

	
	}
	
	// keys based on input schema rather than mapped-to schema
	public List<SecureRelDataTypeField> getMatchableSliceAttributes() {
		LogicalProject project = (LogicalProject) baseRelNode.getRelNode();
		List<RexNode> projExpressions = project.getChildExps();
		
		List<SecureRelDataTypeField> baseKey = children.get(0).getSliceAttributes();
		List<SecureRelDataTypeField> derivedKey = new ArrayList<SecureRelDataTypeField>();
		
		for(SecureRelDataTypeField f : baseKey) {
			
			for(RexNode rex : projExpressions) {
				if(rex.getKind() == SqlKind.INPUT_REF) {
					int ordinal = ((RexInputRef) rex).getIndex();
					
					if(ordinal == f.getBaseField().getIndex()) { // if it is a match
						derivedKey.add(f);
						break;
					}
				}
			}
		}
		return derivedKey;

	
	}

	
	@Override
	public void initializeStatistics() {
		children.get(0).initializeStatistics();
		
		LogicalProject project = (LogicalProject) baseRelNode.getRelNode();
		List<RexNode> projExpressions = project.getChildExps();
		SecureRelRecordType projSchema = getSchema();
		SecureRelRecordType inSchema = getInSchema();
		Iterator<RexNode> rexItr = projExpressions.iterator();
		
		// map each dstField to its source and initialize stats based on that 
		for(SecureRelDataTypeField f : projSchema.getSecureFieldList()) {
			try {
				AttributeResolver.initializeFieldStatistics(rexItr.next(), f, inSchema);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
	}
	


}
