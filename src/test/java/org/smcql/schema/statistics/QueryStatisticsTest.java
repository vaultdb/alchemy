package org.smcql.schema.statistics;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.calcite.plan.RelOptUtil;
import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.schema.statistics.ObliviousFieldStatistics;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

// this only works on example DB from setup.sh
public class QueryStatisticsTest  extends BaseTest {

	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);
	}

	
	public void testDemographicsProjection() throws Exception {
		String query = "SELECT patient_id,gender,birth_year FROM demographics";
		SecureRelRecordType schema = testQuery("demographics-scan", query);
		
		List<ObliviousFieldStatistics> observedStats = new ArrayList<ObliviousFieldStatistics>();
		ObliviousFieldStatistics patientIdStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "patient_id");

		ObliviousFieldStatistics genderStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics","gender");
		// make it oblivious, getExpected only works for public attrs
		genderStats.setDistinctCardinality(3);
		genderStats.setMaxMultiplicity(-1);
		genderStats.setMin(1);
		genderStats.setMax(3);
		List<Long> genderDomain = ObliviousFieldStatistics.generateDomain(1L,  3L);
		genderStats.setDomain(genderDomain);

		ObliviousFieldStatistics birthYearStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "birth_year");
		birthYearStats.setDomain(null);
		birthYearStats.setDistinctCardinality(72);
		birthYearStats.setMaxMultiplicity(-1);
		birthYearStats.setMin(1924);
		birthYearStats.setMax(1995);
		
		logSchemaStats(schema);
		
		for(SecureRelDataTypeField f : schema.getSecureFieldList()) {
			observedStats.add(f.getStatistics());
						
		}
		
		assertEquals(6, schema.getCardinalityBound());
		assertEquals(patientIdStats, observedStats.get(0));
		assertEquals(genderStats, observedStats.get(1));		
		assertEquals(birthYearStats, observedStats.get(2));
		
		
	}
	

	public void testDemographicsFilter() throws Exception {
		String query = "SELECT patient_id from demographics WHERE patient_id = 1";
		SecureRelRecordType schema = testQuery("demographics-filter", query);
		logSchemaStats(schema);

		
		List<Long> domain = new ArrayList<Long>();
		domain.add(1L);
		
		ObliviousFieldStatistics expectedStats = new ObliviousFieldStatistics(null, 1L, 1L, domain, 1L, 1L, 1L);
		ObliviousFieldStatistics observedStats = schema.getSecureField(0).getStatistics();
		
		assertEquals(1, schema.getCardinalityBound());
		assertEquals(expectedStats, observedStats);
	}
	

	public void testDemographicsMultiFilter() throws Exception {
		
		String query = "SELECT patient_id from demographics WHERE gender = 1 AND birth_year = 1990";
		SecureRelRecordType schema = testQuery("demographics-filter", query);
		logSchemaStats(schema);

		ObliviousFieldStatistics expectedStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "patient_id");
		// TODO: check other fields in ObliviousFieldStatistic member variables
		assertEquals(6, schema.getCardinalityBound());
		assertEquals(expectedStats, schema.getSecureField(0).getStatistics());
		
	}

	//delete this
//			public void testDemographicsProjection() throws Exception {
//
//			String query = "SELECT patient_id,gender,birth_year FROM demographics";
//			SecureRelRecordType schema = testQuery("demographics-scan", query);
//
//			List<ObliviousFieldStatistics> observedStats = new ArrayList<ObliviousFieldStatistics>();
//			ObliviousFieldStatistics patientIdStats =
//					ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "patient_id");
//
//			ObliviousFieldStatistics genderStats =
//					ObliviousFieldStatisticsTest.getExpectedOutput("demographics","gender");
//			// make it oblivious, getExpected only works for public attrs
//			genderStats.setDistinctCardinality(3);
//			genderStats.setMaxMultiplicity(-1);
//			genderStats.setMin(1);
//			genderStats.setMax(3);
//			List<Long> genderDomain = ObliviousFieldStatistics.generateDomain(1L,  3L);
//			genderStats.setDomain(genderDomain);
//
//			ObliviousFieldStatistics birthYearStats =
//					ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "birth_year");
//			birthYearStats.setDomain(null);
//			birthYearStats.setDistinctCardinality(72);
//			birthYearStats.setMaxMultiplicity(-1);
//			birthYearStats.setMin(1924);
//			birthYearStats.setMax(1995);
//
//			logSchemaStats(schema);
//
//			for(SecureRelDataTypeField f : schema.getSecureFieldList()) {
//				observedStats.add(f.getStatistics());
//
//			}
//
//			assertEquals(6, schema.getCardinalityBound());
//			assertEquals(patientIdStats, observedStats.get(0));
//			assertEquals(genderStats, observedStats.get(1));
//			assertEquals(birthYearStats, observedStats.get(2));
//
//
//		}

	public void testSimpleJoin() throws Exception {

		String query = "SELECT * FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id";
		SecureRelRecordType schema = testQuery("simple-join", query);

		List<ObliviousFieldStatistics> observedStats = new ArrayList<ObliviousFieldStatistics>();
		ObliviousFieldStatistics patientIdStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "patient_id");

		ObliviousFieldStatistics genderStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics","gender");
		// make it oblivious, getExpected only works for public attrs
		genderStats.setDistinctCardinality(3);
		genderStats.setMaxMultiplicity(-1);
		genderStats.setMin(1);
		genderStats.setMax(3);
		List<Long> genderDomain = ObliviousFieldStatistics.generateDomain(1L,  3L);
		genderStats.setDomain(genderDomain);

		ObliviousFieldStatistics birthYearStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics", "birth_year");
		birthYearStats.setDomain(null);
		birthYearStats.setDistinctCardinality(72);
		birthYearStats.setMaxMultiplicity(-1);
		birthYearStats.setMin(1924);
		birthYearStats.setMax(1995);


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
		// schema: diag concatenated with meds
		// TODO: May, please fill this in with real stats as in testDemographicProjection
		// everything in meds may have up to 4 copies
		// diag statistics remain the same

	}
	
	public void testGroupByAggregate() throws Exception {
		//gender has {0, 1, 2}, which is female, male, and unknown
		//first scan demographics, do aggregate where we group by gender
		//we need the input size will be d  and output cardinality 3
		//we have the get cardinality bound associated with
		//domain gender in our output relation is going to be the same = {0, 1, 2}
		//range for each of those counts range(COUNT) = {0,..,n} and n is the number of input tuples
		//COUNT is every tuple that matches the input criteria
		//[1...6] -> 7 options to choose from but if you have 0 elements of something, you would strike it out as a
		//dummy and then it becomes [1...6]
		String query = "SELECT gender,COUNT(*) FROM demographics GROUP BY gender";
		SecureRelRecordType schema = testQuery("group-by-aggregate", query);
		logSchemaStats(schema);
		
		
		long cardinalityBound = 3;
		ObliviousFieldStatistics genderStats, countStats;
		
		genderStats = ObliviousFieldStatisticsTest.getExpectedOutput("demographics","gender");
		// make it oblivious, getExpected only works for public attrs
		genderStats.setDistinctCardinality(3);
		genderStats.setMaxMultiplicity(-1);
		genderStats.setMin(1);
		genderStats.setMax(3);
		genderStats.setCardinality(3);
		List<Long> genderDomain = ObliviousFieldStatistics.generateDomain(1L,  3L);
		genderStats.setDomain(genderDomain);

		countStats = new ObliviousFieldStatistics(); // the only thing we know is its range
		countStats.setMin(1L);
		countStats.setMax(6L); // can't have a count greater than its input card
		countStats.setCardinality(3);
		
		if(schema.getSecureField(0).getStatistics() != null) {
			cardinalityBound = schema.getCardinalityBound();
		}
		
		assertEquals(3, cardinalityBound);
		// TODO: Nisha, please hand-verify these expected values
		assertEquals(genderStats, schema.getSecureField(0).getStatistics());
		assertEquals(countStats, schema.getSecureField(1).getStatistics());
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
