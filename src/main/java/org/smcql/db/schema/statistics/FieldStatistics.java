package org.smcql.db.schema.statistics;

import java.util.List;

import org.apache.calcite.rel.type.RelDataTypeField;
import org.smcql.plan.SecureRelNode;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.TypeMap;

// to be attached to a SecureRelDataTypeField 
// for use in deriving cardinality bounds 
// for intermediate results
// just supporting integer statistics for now
// TODO: templatize this later with FieldStatistics<T>
public class FieldStatistics { 
	
	private long distinctValues = -1;
	private long maxMultiplicity = -1;
	private List<Long>  domain;
	private long min = -1;
	private long max = -1;

	

	public FieldStatistics() {

	}


	
	// called on public field to automatically collect stats
	// this is for leaf nodes in the query tree
	public FieldStatistics(SecureRelDataTypeField f) {
		String name = f.getName();
		
		// TODO: populate statistics from schema and public vals
		// read in CHECKs from table defs for min/max
		// get max multiplicity, distinct cardinality, etc. for public vals
		
		// model stats building after StatisticsCollector
		
	}


	@Override
    public boolean equals(Object o) { 
		if(o instanceof FieldStatistics) {
			FieldStatistics f = (FieldStatistics) o;
			if(f.distinctValues == this.distinctValues && 
					f.domain == this.domain && 
					f.min == this.min && 
					f.max == this.max &&
					f.maxMultiplicity == this.maxMultiplicity)
				return true;
		}
		return false;
	}
	

	@Override 
	public String toString() {
		String output = "(distinct value count=" + Long.toString(distinctValues) +
					      " max multiplicity=" + Long.toString(maxMultiplicity) + 
					      " range=[" + min + "," + max + "] " +
					      " domain: " + domain + ")";
		return output;
	}
	public long getDistinctValues() {
		return distinctValues;
	}



	public void setDistinctValues(long distinctIDs) {
		this.distinctValues = distinctIDs;
	}



	public long getMaxMultiplicity() {
		return maxMultiplicity;
	}



	public void setMaxMultiplicity(long maxMultiplicity) {
		this.maxMultiplicity = maxMultiplicity;
	}



	public List<Long> getDomain() {
		return domain;
	}



	public void setDomain(List<Long> domain) {
		this.domain = domain;
	}



	public long getMax() {
		return max;
	}



	public void setMax(long max) {
		this.max = max;
	}



	public long getMin() {
		return min;
	}



	public void setMin(long min) {
		this.min = min;
	}
	

	
	
}
