package org.smcql.planner;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.calcite.plan.RelOptUtil;
import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.SecureRelRoot;

public class SecureOperatorTreeTest extends BaseTest {
	
	public void testAspirinCount() throws Exception {
		String expectedTree = "LogicalAggregate-Secure, schema:(#0: rx_cnt BIGINT Public)\n"
				+ "		LogicalMerge-Secure\n"
				+ "			LogicalAggregate-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				+ "				LogicalJoin-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: patient_id0 INTEGER Public,#3: timestamp_0 TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public, #2: patient_id0 INTEGER Public], project='patient_id=[$0]', filter='condition=[LIKE($2, '414%')], condition=[LIKE(LOWER($2), '%aspirin%')]'\n"
				+ "					LogicalMerge-Slice\n"
				+ "						JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: icd9 VARCHAR(7) Protected), project='patient_id=[$0], timestamp_=[$2], icd9=[$7]'\n"
				+ "					LogicalMerge-Slice\n"
				+ "						JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: medication VARCHAR(2147483647) Protected), project='patient_id=[$0], timestamp_=[$2], medication=[$3]'";

		String expectedSql = "SELECT patient_id, timestamp_, medication FROM mi_cohort_medications ORDER BY patient_id";
		
		runTest("aspirin-count", expectedTree, expectedSql);
	
	}
	
	public void testCDiff() throws Exception {
		String expectedTree = "LogicalAggregate-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				+ "		LogicalJoin-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private,#3: patient_id0 INTEGER Public,#4: timestamp_0 TIMESTAMP(6) Private,#5: w0$o00 INTEGER Private), slice key: [#0: patient_id INTEGER Public, #3: patient_id0 INTEGER Public], project='patient_id=[$0]'\n"
				+ "			LogicalWindow-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private), slice key: [#0: patient_id INTEGER Public], 'filter=condition=[=(icd9, '008.45')'\n"
				+ "				LogicalMerge-Slice\n"
				+ " 				JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: icd9 VARCHAR(4) Protected), project='patient_id=[$0], timestamp_=[$1], icd9=[$7]'\n"
				+ "			LogicalWindow-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private), slice key: [#0: patient_id INTEGER Public], 'filter=condition=[=(icd9, '008.45')'\n"
				+ "				LogicalMerge-Slice\n"
				+ "					JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: icd9 VARCHAR(4) Protected), project='patient_id=[$0], timestamp_=[$1], icd9=[$7]'";
	
		String expectedSql = "SELECT patient_id, timestamp_, icd9 FROM cdiff_cohort_diagnoses ORDER BY patient_id, timestamp_";
		
		runTest("cdiff", expectedTree, expectedSql);
	}
	
	public void testComorbidity() throws Exception {
		String expectedTree = "LogicalSort-Secure, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n" 
				+ "		LogicalAggregate-Secure, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				+ "			LogicalMerge-Secure\n"
				+ "				LogicalAggregate-Secure, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				+ "					JdbcTableScan-Plain(table=[[cdiff_cohort_diagnoses]], project='major_icd9=[$8]')";
		
		String expectedSql = "SELECT major_icd9, COUNT(*) FROM cdiff_cohort_diagnoses GROUP BY major_icd9 ORDER BY major_icd9";
		
		runTest("comorbidity", expectedTree, expectedSql);
	}
	

	void runTest(String testName, String expectedTree, String expectedSql) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		Logger logger = SystemConfiguration.getInstance().getLogger();

		String sql = super.readSQL(testName);
		logger.log(Level.INFO, "Parsing " + sql);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
		
		QueryCompiler qc = new QueryCompiler(secRoot);
		ExecutionStep root = qc.getRoot();
		
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		//assertEquals(expectedTree, testTree);
	}
}
