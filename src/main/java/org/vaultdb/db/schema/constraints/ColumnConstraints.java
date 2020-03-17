package org.vaultdb.db.schema.constraints;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.calcite.rel.type.RelDataTypeFieldImpl;
import org.apache.calcite.sql.type.SqlTypeName;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.Field;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.type.SecureRelDataTypeField.SecurityPolicy;
import org.vaultdb.util.Utilities;

// to be attached to a SecureRelDataTypeField 
// for use in deriving cardinality bounds 
// for intermediate results




public class ColumnConstraints<T> { 
	
	private long distinctValues = -1;
	private long maxMultiplicity = -1;
	private List<T>  domain;
	private T min = null;
	private T max = null;
	private long cardinality = -1; // # of tuples in relation
	private SecureRelDataTypeField columnDefinition = null;
	
	// pushing key constraints up to the table level since they can be composite
	boolean isUnique = false;
	
	public ColumnConstraints() {

	}


	
	// called on public field to automatically collect stats
	// this is for leaf nodes in the query tree
	public ColumnConstraints(SecureRelDataTypeField f) throws Exception  {
		columnDefinition = f;
		
		String table = columnDefinition.getStoredTable();
		String attr = columnDefinition.getStoredAttribute();
		
		assert(table != null && attr != null);
		
		
		if(columnDefinition.getSecurityPolicy() == SecurityPolicy.Public)  {	
			populatePublicAttribute();
		}
		

		
	}

	// constructor for intermediate results and test cases
	public ColumnConstraints(SecureRelDataTypeField f, Long distinctCardinality, Long aMaxMultiplicity, 
			List<T> aDomain, T aMinValue, T aMaxValue, Long aCardinality) {

		
		distinctValues = distinctCardinality;
		maxMultiplicity = aMaxMultiplicity;
		domain = aDomain;
		min = aMinValue;
		max = aMaxValue;
		cardinality = aCardinality;	
	}
	
	// copy constructor for new SecureRelDataTypeField
	public ColumnConstraints(ColumnConstraints<T> src)		
		{
			columnDefinition = src.columnDefinition;
			distinctValues = src.distinctValues;
			maxMultiplicity = src.maxMultiplicity;
			min = src.min;	
			max = src.max;
			cardinality = src.cardinality;
			
			if(src.domain != null)
				domain = new ArrayList<T>(src.domain);

		}
		
	// copy constructor for new SecureRelDataTypeField, i.e., the parent of an existing one
	
	public ColumnConstraints(SecureRelDataTypeField f, ColumnConstraints<T> src)		
	{
		columnDefinition = f;
		distinctValues = src.distinctValues;
		maxMultiplicity = src.maxMultiplicity;
		min = src.min;	
		max = src.max;
		cardinality = src.cardinality;
		
		// type needs to match up
		assert(f.getType().equals(src.columnDefinition.getType()));
		
		if(src.domain != null)
			domain = new ArrayList<T>(src.domain);
		// TODO: check if conditions still hold for auto-generated domains


	}
	




	public static ColumnConstraints getCachedStatistics(String table, String attr) throws Exception {
		ColumnConstraints f = new ColumnConstraints();
		f.populatePrivateAttribute(table, attr);
		return f;
	}
	
	

	@Override
    public boolean equals(Object o) { 
		if(o instanceof ColumnConstraints) {

			ColumnConstraints f = (ColumnConstraints) o;
			if(f.distinctValues == this.distinctValues && 
					f.min == this.min && 
					f.max == this.max &&
					f.maxMultiplicity == this.maxMultiplicity &&
					f.cardinality == this.cardinality)
					if((f.domain == null && domain == null ) || 
							(f.domain != null && domain != null && f.domain.equals(domain)))
						return true;
		}
		return false;
	}
	

	@Override 
	public String toString() {
		String output = "(distinct value count=" + Long.toString(distinctValues) +
					      " max multiplicity=" + Long.toString(maxMultiplicity) + 
					      " range=(" + min + "," + max + ") " +
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
		
		// TODO: check if conditions still hold for auto-generated domains
	}


	
	public long getMaxMultiplicity() {
		return maxMultiplicity;
	}



	public void setMaxMultiplicity(long maxMultiplicity) {
		this.maxMultiplicity = maxMultiplicity;
	}



	public List<T> getDomain() {
		return domain;
	}



	public void setDomain(List<T> aDomain) {
		if(aDomain == null)
			domain = null;
		else
			this.domain = new ArrayList<T>(aDomain);
	}



	public T getMax() {
		return max;
	}



	public void setMax(T max) {
		this.max = max;
	}



	public T getMin() {
		return min;
	}



	public void setMin(T min) {
		this.min = min;
	}
	


	public void populatePublicAttribute() throws Exception {
		
		
		String table = columnDefinition.getStoredTable();
		String attr = columnDefinition.getStoredAttribute();
		
		maxMultiplicity = runScalarIntegerQuery("SELECT COUNT(*) FROM " +  table + " GROUP BY " + attr + " ORDER BY COUNT(*) DESC LIMIT 1");
		// this may be suboptimal for dbs with big domains - TODO: lazily evaluate this later
		domain = runListQuery("SELECT DISTINCT " + attr + " FROM " + table);
		min = runScalarQuery("SELECT min(" + attr + ") FROM " + table);
		max = runScalarQuery("SELECT max(" + attr + ") FROM " + table);
		cardinality = runScalarIntegerQuery("SELECT COUNT(*) FROM " + table);
		distinctValues = domain.size();
		
	}
	
	
	private List<T> runListQuery(String query) {

		return null;
	}



	// for use only with ints
	public  List<T> generateObliviousDomain(T minValue, T maxValue) {
		

		if(columnDefinition.getType().getSqlTypeName() == SqlTypeName.BIGINT || 
				columnDefinition.getType().getSqlTypeName() == SqlTypeName.INTEGER) {
			
			List<Long> localDomain =  new ArrayList<Long>();
			Long min = (Long) minValue;
			Long max = (Long) maxValue;
			for(Long i = min; i <= max; ++i) {
					localDomain.add(i);
				}
			return (List<T>) localDomain;
		}
		
		// all other cases
		return null;

	}
	
	
	private void populatePrivateAttribute(String table, String attr) throws Exception {
		
		/*  Deprecated: JMR return to this
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

			// when it is possible to infer the domain, we take the worst-case
			if((max - min +1) == distinctValues && cardinality >= distinctValues) {
			     // domain is known, fill it in
				// TODO: make this faster by not materializing it and populating it as needed
				domain = generateObliviousDomain(min, max);
			}
		}*/
	}
	
	// helper methods
	@SuppressWarnings("rawtypes")
	protected T runScalarQuery(String query) throws Exception {
		// queries the unioned dataset
		QueryTable result = SqlQueryExecutor.query(query);

		Tuple firstTuple = result.getTuple(0);
		Field f =  firstTuple.getField(0);
		return (T) f.getValue();
	}
	
	protected long runScalarIntegerQuery(String query) throws Exception {
		
		QueryTable result = SqlQueryExecutor.query(query);

		Tuple firstTuple = result.getTuple(0);
		IntField f =  (IntField) firstTuple.getField(0);
		return f.getValue();
		
		
	}

	
	
	
}
