package org.vaultdb.optimizer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.vaultdb.BaseTest;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.db.data.field.IntField;
import org.vaultdb.db.schema.constraints.ColumnConstraints;
import org.vaultdb.db.schema.constraints.ColumnConstraintsFactory;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

public class ObliviousFieldStatisticsTest  extends BaseTest  {

	protected void setUp() throws Exception {
		String setupFile = Utilities.getVaultDBRoot()+ "/conf/setup.localhost";
		System.setProperty("vaultdb.setup", setupFile);
	}

	
	public void testDiagnosesPid() throws Exception { 
		String table = "diagnoses";
		String attr = "patient_id";
		
		ColumnConstraints<Long> stats = getExpectedStatistics(table, attr);
		testCase(table, attr, stats);	
	}
	
	public void testMedicationsPid() throws Exception { 
		String table = "medications";
		String attr = "patient_id";
		
		ColumnConstraints<Long> stats = getExpectedStatistics(table, attr);
		testCase(table, attr, stats);	
	}
	
	//  this test demos how to hand private attrs likedemographic.birth_year
	// where we have constraints that tell us the attribute's range, but 
	// can glean no additional information
	public void testDemographicsBirthYear() throws Exception {
		String table = "demographics";
		String attr = "birth_year";
		
		ColumnConstraints<Long> stats = (ColumnConstraints<Long>) ColumnConstraintsFactory.get(table, attr);
		
		stats.setMin(1924L);
		stats.setMax(1995L);
		stats.setDistinctCardinality(72L);
		stats.setCardinality(6L);
		
		testCase(table, attr, stats);
	}

	public void testMedicationsMonth() throws Exception {
		String table = "medications";
		String attr = "month";
		
		ColumnConstraints<Long> expectedStats = (ColumnConstraints<Long>) ColumnConstraintsFactory.get(table, attr);

		expectedStats.setMin(1L);
		expectedStats.setMax(12L);
		expectedStats.setDistinctCardinality(12);
		expectedStats.setCardinality(4);

		testCase(table, attr, expectedStats);
		
		
	}
	
	public void testDemographicsGender() throws Exception {
		String table = "demographics";
		String attr = "gender";
		
		ColumnConstraints expectedStats = (ColumnConstraints<Long>) ColumnConstraintsFactory.get(table, attr);
		// TODO: Nisha and May, fill in expected output following the pattern in testDemographicsBirthYear
		// Since attr is not public, may only use contents of relation_statistics in test database to get stats
		// gender should have  cardinality of <demo table length>, range = 1..3, distinct vals: 3, ...

		expectedStats.setMin(1);
		expectedStats.setMax(3);
		List<Long> d = new ArrayList<>(Arrays.asList(1L, 2L, 3L));
		expectedStats.setDomain(d);
		expectedStats.setCardinality(6);
		expectedStats.setDistinctCardinality(3);

		testCase(table, attr, expectedStats);
		
	}

	public void testDiagnosesMajorIcd9() throws Exception {
		String table = "diagnoses";
		String attr = "major_icd9";
		
		ColumnConstraints expectedStats = (ColumnConstraints<Long>) ColumnConstraintsFactory.get(table, attr);
		// icd9 should have  cardinality of <diag table length>, 1604 distinct vals
		// TODO: Nisha and May, derive expected output following the pattern in testDemographicsBirthYear

		expectedStats.setCardinality(16);
		expectedStats.setDistinctCardinality(1604);

		testCase(table, attr, expectedStats);
		
	}
	
	
	// For use with public attributes
	// only supporting Longs for this test
	public static ColumnConstraints<Long> getExpectedStatistics(String table, String attr) throws Exception {

		ColumnConstraints<Long> stats = (ColumnConstraints<Long>) ColumnConstraintsFactory.get(table, attr);
		
		long maxMultiplicity = runLongIntQuery("SELECT COUNT(*) FROM " +  table + " GROUP BY " + attr + " ORDER BY COUNT(*) DESC LIMIT 1");
		List<Long> domain = runLongIntListQuery("SELECT DISTINCT " + attr + " FROM " + table);
		long min = runLongIntQuery("SELECT min(" + attr + ") FROM " + table);
		long max = runLongIntQuery("SELECT max(" + attr + ") FROM " + table);
		long cardinality = runLongIntQuery("SELECT COUNT(*) FROM " + table);
		
		stats.setMaxMultiplicity((int) maxMultiplicity); 
		stats.setDomain(domain);
		stats.setDistinctCardinality(domain.size());
		stats.setMax(max);
		stats.setMin(min);
		stats.setCardinality(cardinality);
		
		return stats;
	}

	protected static long runLongIntQuery(String query) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
	
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		
		String distributedQuery = Utilities.getDistributedQuery(query);
	
		QueryTable result =  SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
		Tuple firstTuple = result.getTuple(0);
		IntField f = (IntField) firstTuple.getField(0);
		return f.getValue();
	}

	// caution: this only works for one-relation queries, i.e., ones with no joins	
	protected static List<Long> runLongIntListQuery(String query) throws Exception {
		String aliceId = ConnectionManager.getInstance().getAlice();
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		String distributedQuery = Utilities.getDistributedQuery(query);

		System.out.println("Initial query: " + query);
		System.out.println("Distributed query: " + distributedQuery);
		
		QueryTable output =  SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
		
		List<Long> results = new ArrayList<Long>();
		for(Tuple t : output.tuples()) {
			IntField f = (IntField) t.getField(0);
			results.add(f.getValue());
		}
		Collections.sort(results);
		return results;
	}
	
	protected void testCase(String table, String attr, ColumnConstraints expectedOutput) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();
		
	
		SecureRelDataTypeField field = Utilities.lookUpAttribute(table, attr);
		field.initializeConstraints();
		
		logger.log(Level.INFO, "Expected output: " + expectedOutput);	
		logger.log(Level.INFO, "observed output: " + field.getColumnConstraints());
		assertEquals(expectedOutput, field.getColumnConstraints());
	}
	

}
