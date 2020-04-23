package org.vaultdb.db.schema.constraints;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.vaultdb.type.SecureRelDataTypeField;

// keep track of column references
// maintains integrity constraints for one table
// links it back to column definitions so that we can easily look up column constraints using SecureRelRecordType

public class IntegrityConstraints {
	
	// for each integrity constraint
	List<
		// map its source column --> target
	  	Map<SecureRelDataTypeField, SecureRelDataTypeField>>  columnReferences;
	
	
	
	public IntegrityConstraints(String aName) {
		columnReferences = new ArrayList<Map<SecureRelDataTypeField, SecureRelDataTypeField>>();
	}

	
	
	
	public void addConstraint(Map<SecureRelDataTypeField, SecureRelDataTypeField> constraint) {
		columnReferences.add(constraint);
	}

	public int getConstraintCount() {
		return columnReferences.size();
	}
	
	public Map<SecureRelDataTypeField, SecureRelDataTypeField> getConstaint(int i) {
		return columnReferences.get(i);
	}
}

