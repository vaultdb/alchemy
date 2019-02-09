package org.smcql.schema.statistics;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.calcite.plan.RelOptUtil;
import org.junit.Test;
import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.db.schema.statistics.ObliviousFieldStatistics;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;
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
		
		ObliviousFieldStatistics stats = getExpectedOutput(table, attr);
		testCase(table, attr, stats);	
	}
	
	public void testMedicationsPid() throws Exception { 
		String table = "medications";
		String attr = "patient_id";
		
		ObliviousFieldStatistics stats = getExpectedOutput(table, attr);
		testCase(table, attr, stats);	
	}
	
	//  this test demos how to hand private attrs likedemographic.birth_year
	// where we have constraints that tell us the attribute's range, but 
	// can glean no additional information
	public void testDemographicsBirthYear() throws Exception {
		String table = "demographics";
		String attr = "birth_year";
		
		ObliviousFieldStatistics stats = new ObliviousFieldStatistics();
		// TODO: get correct inputs from DB
		stats.setMin(1924);
		stats.setMax(1995);
		stats.setDistinctCardinality(72);
		stats.setCardinality(6);
		
		testCase(table, attr, stats);
		
		
	}
	
	public void testDemographicsScan() throws Exception {
		String query = "SELECT patient_id,gender,birth_year FROM demographics";
	
		SecureRelRecordType schema = testQuery("demographics-scan", query);
		List<ObliviousFieldStatistics> expectedStats = new ArrayList<ObliviousFieldStatistics>();
		List<ObliviousFieldStatistics> observedStats = new ArrayList<ObliviousFieldStatistics>();
		
		// TODO: fill this in with real vals
		ObliviousFieldStatistics patientIdStats = new ObliviousFieldStatistics();
		patientIdStats.setMaxMultiplicity(1);
		
		ObliviousFieldStatistics genderStats = new ObliviousFieldStatistics();
		ObliviousFieldStatistics birthYearStats = new ObliviousFieldStatistics();
		
		expectedStats.add(patientIdStats);
		expectedStats.add(genderStats);
		expectedStats.add(birthYearStats);
				
		for(SecureRelDataTypeField f : schema.getSecureFieldList()) {
			observedStats.add(f.getStatistics());
						
		}
		
		assertEquals(expectedStats, observedStats);
		
		logSchemaStats(schema);
		
	}
	
	public void testDemographicsProjection() throws Exception {
		String query = "SELECT patient_id FROM demographics";
	
		SecureRelRecordType schema = testQuery("demographics-pid", query);
		logSchemaStats(schema);
	}
	
	public void testDemographicsFilter() throws Exception {
		String query = "SELECT patient_id from demographics WHERE gender = 1";
		SecureRelRecordType schema = testQuery("demographics-filter", query);
		logSchemaStats(schema);

		// expect output cardinality of 6
	}
	
	
	protected void logSchemaStats(SecureRelRecordType schema) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();
		logger.log(Level.INFO, "Schema: " + schema);
		
		for(SecureRelDataTypeField f : schema.getSecureFieldList()) {
			logger.log(Level.INFO, "Field: " + f.toString() + " has statistics " + f.getStatistics());
		}
	}
	protected ObliviousFieldStatistics getExpectedOutput(String table, String attr) throws Exception {
		
		
		
		ObliviousFieldStatistics stats = new ObliviousFieldStatistics();
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

	// caution: this only works for one-relation queries, i.e., ones with no joins	
	protected List<Long> runLongIntListQuery(String query) throws Exception {
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
	
	protected void testCase(String table, String attr, ObliviousFieldStatistics expectedOutput) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();
		
	
		SecureRelDataTypeField field = Utilities.lookUpAttribute(table, attr);
		field.initializeStatistics();
		
		logger.log(Level.INFO, "Expected output: " + expectedOutput);	
		logger.log(Level.INFO, "observed output: " + field.getStatistics());
		assertEquals(expectedOutput, field.getStatistics());
	}
	
	protected SecureRelRecordType testQuery(String testName, String sql) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		Logger logger = SystemConfiguration.getInstance().getLogger();

		logger.log(Level.INFO, "Parsing " + sql);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		Operator planRoot = secRoot.getPlanRoot();
		planRoot.initializeStatistics();
		
	
		logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
		
		QueryCompiler qc = new QueryCompiler(secRoot);
		ExecutionStep root = qc.getRoot();
		
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		return secRoot.getPlanRoot().getSchema();

	}

}
