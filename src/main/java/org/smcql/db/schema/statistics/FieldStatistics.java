package org.smcql.db.schema.statistics;

import java.util.List;

import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.TypeMap;

// to be attached to a SecureRelDataTypeField 
// for use in deriving cardinality bounds 
// for intermediate results

public class FieldStatistics<T> {
	
	int distinctValues;
	int maxMultiplicity;
	List<T>  domain;
	T min;
	T max;

	

	// called on public field to automatically collect stats
	public FieldStatistics(SecureRelDataTypeField<T> f) {
	
		// TODO: populate statistics from schema and public vals
		// read in CHECKs from table defs for min/max
		// get max multiplicity, distinct cardinality, etc. for public vals
		
	}
	
	// create a table in the DB called stats from which we draw the rest of the fields
	// can't do that easily b/c of the T values above.
	// need to create a text file once
	
	// orthogonal TODO: rename merge to union
	
	
}
