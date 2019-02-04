package org.smcql.db.schema.statistics;

import java.util.List;

import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.TypeMap;

// to be attached to a SecureRelDataTypeField 
// for use in deriving cardinality bounds 
// for intermediate results
// just supporting integer statistics for now
// TODO: templatize this later with FieldStatistics<T>
public class FieldStatistics { 
	
	private int distinctValues;
	private int maxMultiplicity;
	private List<Integer>  domain;
	private int min;
	private int max;

	

	// called on public field to automatically collect stats
	public FieldStatistics(SecureRelDataTypeField f) {
		String name = f.getName();
		
		// TODO: populate statistics from schema and public vals
		// read in CHECKs from table defs for min/max
		// get max multiplicity, distinct cardinality, etc. for public vals
		
		// model stats building after StatisticsCollector
		
	}



	public int getDistinctValues() {
		return distinctValues;
	}



	public void setDistinctValues(int distinctValues) {
		this.distinctValues = distinctValues;
	}



	public int getMaxMultiplicity() {
		return maxMultiplicity;
	}



	public void setMaxMultiplicity(int maxMultiplicity) {
		this.maxMultiplicity = maxMultiplicity;
	}



	public List<Integer> getDomain() {
		return domain;
	}



	public void setDomain(List<Integer> domain) {
		this.domain = domain;
	}



	public int getMax() {
		return max;
	}



	public void setMax(int max) {
		this.max = max;
	}



	public int getMin() {
		return min;
	}



	public void setMin(int min) {
		this.min = min;
	}
	

	
	
}
