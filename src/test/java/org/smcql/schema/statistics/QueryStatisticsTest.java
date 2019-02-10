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
public class QueryStatisticsTest extends ObliviousFieldStatisticsTest  {

	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}

	
	public void testDemographicsProjection() throws Exception {
		String query = "SELECT patient_id,gender,birth_year FROM demographics";
		SecureRelRecordType schema = testQuery("demographics-scan", query);
		
		List<ObliviousFieldStatistics> expectedStats = new ArrayList<ObliviousFieldStatistics>();
		List<ObliviousFieldStatistics> observedStats = new ArrayList<ObliviousFieldStatistics>();
		
		ObliviousFieldStatistics patientIdStats = getExpectedOutput("demographics", "patient_id");

		// TODO: Nisha, please fill this in with real vals from union of testDBs
		// These figures are available in the relation_statistics table in the db smcql_test_site1
		// use methods like ObliviousFieldStatistics.setMax
		// the test will complete successfully after this
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
	

	public void testDemographicsFilter() throws Exception {
		String query = "SELECT patient_id from demographics WHERE patient_id = 1";
		SecureRelRecordType schema = testQuery("demographics-filter", query);
		logSchemaStats(schema);

		// TODO: check other fields in ObliviousFieldStatistic member variables
		assertEquals(1, schema.getCardinalityBound());
	}
	

	public void testDemographicsMultiFilter() throws Exception {
		
		String query = "SELECT patient_id from demographics WHERE gender = 1 AND birth_year = 1990";
		SecureRelRecordType schema = testQuery("demographics-filter", query);
		logSchemaStats(schema);

		// TODO: check other fields in ObliviousFieldStatistic member variables
		assertEquals(6, schema.getCardinalityBound());

	}
	
	public void testSimpleJoin() throws Exception {
		String query = "SELECT * FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id";
		SecureRelRecordType schema = testQuery("simple-join", query);
		logSchemaStats(schema);
		// Medications pid stats: (distinct value count=4 max multiplicity=1 range=[1,6]  domain: {1, 3, 5, 6} cardinality: 4)
		// Diagnoses pid stats:  (distinct value count=6 max multiplicity=4 range=[1,6]  domain: {1, 2, 3, 4, 5, 6} cardinality: 16)
		// intersection set: {1, 3, 5, 6}, |intersection set| = 4
		// match max multiplicity: 4*1
		// expected size: |intersection set| * 4 * 1 = 16
		
		long cardinalityBound = -1;
		
		if(schema.getSecureField(0).getStatistics() != null) {
			cardinalityBound = schema.getCardinalityBound();
		}
		
		assertEquals(16, cardinalityBound);
		
	}
	
	public void testGroupByAggregate() throws Exception {
		String query = "SELECT gender,COUNT(*) FROM demographics GROUP BY gender";
		SecureRelRecordType schema = testQuery("group-by-aggregate", query);
		logSchemaStats(schema);
		
		
		long cardinalityBound = -1;
		
		if(schema.getSecureField(0).getStatistics() != null) {
			cardinalityBound = schema.getCardinalityBound();
		}
		
		assertEquals(3, cardinalityBound);
		
		
		
	}
	
	
	protected void logSchemaStats(SecureRelRecordType schema) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();
		logger.log(Level.INFO, "Schema: " + schema);
		
		for(SecureRelDataTypeField f : schema.getSecureFieldList()) {
			logger.log(Level.INFO, "Field: " + f.toString() + " has statistics " + f.getStatistics());
		}
	}

	
	protected SecureRelRecordType testQuery(String testName, String sql) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		SystemConfiguration.getInstance().setProperty("code-generator-mode", "debug");
		Logger logger = SystemConfiguration.getInstance().getLogger();

		logger.log(Level.INFO, "Parsing " + sql);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		Operator planRoot = secRoot.getPlanRoot();
		planRoot.initializeStatistics();
		
		// logical representation with all ops displayed at finest granularity 
		logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
		
		QueryCompiler qc = new QueryCompiler(secRoot);
		ExecutionStep root = qc.getRoot();
		
		// executable format with filters and projects merged to reduce overhead
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		return secRoot.getPlanRoot().getSchema();

	}

}
