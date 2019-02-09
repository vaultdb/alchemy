package org.smcql.db.schema.statistics;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.calcite.sql.type.SqlTypeName;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;
import org.smcql.util.Utilities;

// to be attached to a SecureRelDataTypeField 
// for use in deriving cardinality bounds 
// for intermediate results
// just supporting integer statistics for now
// TODO: templatize this later with FieldStatistics<T>
public class ObliviousFieldStatistics { 
	
	private long distinctValues = -1;
	private long maxMultiplicity = -1;
	private List<Long>  domain;
	private long min = -1;
	private long max = -1;
	private long cardinality = -1; // # of tuples in relation
	private SecureRelDataTypeField parentField = null;
	

	public ObliviousFieldStatistics() {

	}


	
	// called on public field to automatically collect stats
	// this is for leaf nodes in the query tree
	public ObliviousFieldStatistics(SecureRelDataTypeField f)  {
		parentField = f;
		
		String table = parentField.getStoredTable();
		String attr = parentField.getStoredAttribute();
		
		assert(table != null && attr != null);
		
		SqlTypeName fieldType = parentField.getType().getSqlTypeName();
		// only integer types are supported
		assert(fieldType == SqlTypeName.INTEGER || fieldType == SqlTypeName.BIGINT); 
		
		try {
			if(parentField.getSecurityPolicy() == SecurityPolicy.Public)  {	
				populatePublicAttribute(table, attr);
			}
			else {
				// populate it with cached statistics
				// TODO: collect statistics from schema and public vals
				// read in CHECKs from table defs for min/max
				// get max multiplicity, distinct cardinality, etc. for public vals
				populatePrivateAttribute(table, attr);
			}
		
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		

		
	}

	// constructor for intermediate results
	public ObliviousFieldStatistics(SecureRelDataTypeField f, Long distinctCardinality, Long aMaxMultiplicity, 
			List<Long> aDomain, Long aMinValue, Long aMaxValue, Long aCardinality) {

		// only integer types are supported		
		SqlTypeName fieldType = parentField.getType().getSqlTypeName();
		assert(fieldType == SqlTypeName.INTEGER || fieldType == SqlTypeName.BIGINT); 
		
		distinctValues = distinctCardinality;
		maxMultiplicity = aMaxMultiplicity;
		domain = aDomain;
		min = aMinValue;
		max = aMaxValue;
		cardinality = aCardinality;	
	}
	
	// copy constructor for new SecureRelDataTypeField, i.e., the parent of an existing one
	public ObliviousFieldStatistics(SecureRelDataTypeField f, ObliviousFieldStatistics src) {
		parentField = f;
		distinctValues = src.distinctValues;
		maxMultiplicity = src.maxMultiplicity;
		domain = src.domain;
		min = src.min;	
		max = src.max;
		cardinality = src.cardinality;
	}
	
	public static ObliviousFieldStatistics getCachedStatistics(String table, String attr) throws Exception {
		ObliviousFieldStatistics f = new ObliviousFieldStatistics();
		f.populatePrivateAttribute(table, attr);
		return f;
	}
	
	

	@Override
    public boolean equals(Object o) { 
		if(o instanceof ObliviousFieldStatistics) {

			ObliviousFieldStatistics f = (ObliviousFieldStatistics) o;
			if(f.distinctValues == this.distinctValues && 
					f.min == this.min && 
					f.max == this.max &&
					f.maxMultiplicity == this.maxMultiplicity &&
					f.cardinality == this.cardinality)
					if((f.domain == null && domain == null ) || f.domain.equals(domain))
						return true;
		}
		return false;
	}
	

	@Override 
	public String toString() {
		String output = "(distinct value count=" + Long.toString(distinctValues) +
					      " max multiplicity=" + Long.toString(maxMultiplicity) + 
					      " range=[" + min + "," + max + "] " +
					      " domain: " + domain + " cardinality: " + cardinality + ")";
		return output;
	}
	
	
	public long getDistinctCardinality() {
		return distinctValues;
	}



	public void setDistinctCardinality(long distinctIDs) {
		this.distinctValues = distinctIDs;
	}


	public long getCardinality() {
		return cardinality;
	}



	public void setCardinality(long aCardinality) {
		this.cardinality = aCardinality;
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
	


	private void populatePublicAttribute(String table, String attr) throws Exception {
		
		
		maxMultiplicity = runLongIntQuery("SELECT COUNT(*) FROM " +  table + " GROUP BY " + attr + " ORDER BY COUNT(*) DESC LIMIT 1");
		domain = runLongIntListQuery("SELECT DISTINCT " + attr + " FROM " + table);
		min = runLongIntQuery("SELECT min(" + attr + ") FROM " + table);
		max = runLongIntQuery("SELECT max(" + attr + ") FROM " + table);
		cardinality = runLongIntQuery("SELECT COUNT(*) FROM " + table);
		distinctValues = domain.size();
		
	}
	
	
	
	// TODO: fix this so that null is interpreted as -1 instead of 0 for all intfields
	private void populatePrivateAttribute(String table, String attr) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
		String query = new String("SELECT distinct_values, max_multiplicity, min_value, max_value FROM relation_statistics WHERE relation = \'" + table + "\' AND attr=\'" + attr + "\'");
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		QueryTable result = SqlQueryExecutor.query(query, outSchema, aliceId);
		if(result.tupleCount() > 0) {
			Tuple firstTuple = result.getTuple(0);
		
			// custom null-handling
			// NULL == -1 here
			distinctValues =  (((IntField) firstTuple.getField(0)).getValue() == 0) ? -1 : ((IntField) firstTuple.getField(0)).getValue();
			maxMultiplicity = (((IntField) firstTuple.getField(1)).getValue() == 0) ? -1 : ((IntField) firstTuple.getField(1)).getValue();
			min =  (((IntField) firstTuple.getField(2)).getValue() == 0) ? -1 : ((IntField) firstTuple.getField(2)).getValue();
			max =  (((IntField) firstTuple.getField(3)).getValue() == 0) ? -1 : ((IntField) firstTuple.getField(3)).getValue();
			cardinality = runLongIntQuery("SELECT COUNT(*) FROM " + table);
		// domain unknown for now
		}
	}
	
	// helper methods
	protected long runLongIntQuery(String query) throws Exception {
		QueryTable result = runQuery(query);
		Tuple firstTuple = result.getTuple(0);
		IntField f = (IntField) firstTuple.getField(0);
		return f.getValue();
	}

	// caution: this only works for one-relation queries, i.e., ones with no joins	
	protected List<Long> runLongIntListQuery(String query) throws Exception {
		List<Long> results = new ArrayList<Long>();
		QueryTable output = runQuery(query);
		
		for(Tuple t : output.tuples()) {
			IntField f = (IntField) t.getField(0);
			results.add(f.getValue());
		}
		
		Collections.sort(results);
		return results;
	}
	
	// caution: this only works for one-relation queries, i.e., ones with no joins!
	private QueryTable runQuery(String query) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
		
		// bailing because of recursive dependency with schema inference and FieldStatistics
		// Need to separate this into two passes, to get FieldStatistics out of SecureRelDataTypeField constructor
		// this will work once we stand everything up, but until then...
		
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		String distributedQuery = Utilities.getDistributedQuery(query);
		
		return SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
		

	}
	
	
}
