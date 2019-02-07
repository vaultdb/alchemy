package org.smcql.schema.statistics;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.junit.Test;
import org.smcql.BaseTest;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.db.schema.statistics.FieldStatistics;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

// this only works on example DB from setup.sh
public class TableScanTest extends BaseTest  {

	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}

	
	public void testDiagnosesPid() throws Exception { 
		String table = "diagnoses";
		String attr = "patient_id";
		
		FieldStatistics stats = getExpectedOutput(table, attr);
		testCase(table, attr, stats);	
	}
	
	public void testMedicationsPid() throws Exception { 
		String table = "medications";
		String attr = "patient_id";
		
		FieldStatistics stats = getExpectedOutput(table, attr);
		testCase(table, attr, stats);	
	}
	
	//  this test demos how to hand private attrs likedemographic.birth_year
	// where we have constraints that tell us the attribute's range, but 
	// can glean no additional information
	public void testDemographicsBirthYear() throws Exception {
		String table = "demographics";
		String attr = "birth_year";
		
		FieldStatistics stats = new FieldStatistics();
		stats.setMin(1900);
		stats.setMax(2019);
		
		
		testCase(table, attr, stats);
		
		
	}	
	
	protected FieldStatistics getExpectedOutput(String table, String attr) throws Exception {
		
		
		
		FieldStatistics stats = new FieldStatistics();
		long maxMultiplicity = runLongIntQuery("SELECT COUNT(*) FROM " +  table + " GROUP BY " + attr + " ORDER BY COUNT(*) LIMIT 1");
		List<Long> domain = runLongIntListQuery("SELECT DISTINCT " + attr + " FROM " + table);
		long min = runLongIntQuery("SELECT min(" + attr + ") FROM " + table);
		long max = runLongIntQuery("SELECT max(" + attr + ") FROM " + table);
		
		
		stats.setMaxMultiplicity((int) maxMultiplicity); 
		stats.setDomain(domain);
		stats.setDistinctValues(domain.size());
		stats.setMax(max);
		stats.setMin(min);
		
		
		return stats;
	}

	// caution: this only works for one-relation queries, i.e., ones with no joins
	protected long runLongIntQuery(String query) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
	
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		
		String distributedQuery = Utilities.getDistributedQuery(query);
	
		QueryTable result =  SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
		Tuple firstTuple = result.getTuple(0);
		IntField f = (IntField) firstTuple.getField(0);
		return f.getValue();
	}

	
	protected List<Long> runLongIntListQuery(String query) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		String distributedQuery = Utilities.getDistributedQuery(query);
		
		QueryTable output =  SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
		
		List<Long> results = new ArrayList<Long>();
		for(Tuple t : output.tuples()) {
			IntField f = (IntField) t.getField(0);
			results.add(f.getValue());
		}
		
		return results;
	}
	
	protected void testCase(String table, String attr, FieldStatistics expectedOutput) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();
		
		logger.log(Level.INFO, "Expected output: " + expectedOutput);

		SecureRelDataTypeField field = Utilities.lookUpAttribute(table, attr);
		assertEquals(expectedOutput, field);
	}

}
