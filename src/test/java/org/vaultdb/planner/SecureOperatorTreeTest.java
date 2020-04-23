package org.vaultdb.planner;

import java.util.logging.Level;
import org.apache.calcite.plan.RelOptUtil;
import org.vaultdb.BaseTest;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.plan.SecureRelRoot;

public class SecureOperatorTreeTest extends BaseTest {
	
	protected void setUp() throws Exception {
		super.setUp();
	}
	
	
	public void testAspirinCount() throws Exception {
		String expectedTree = "LogicalAggregate-DistributedOblivious, schema:(#0: rx_cnt BIGINT Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "    LogicalAggregate-DistributedOblivious, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "        LogicalJoin-DistributedOblivious, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(3) Private,#2: patient_id0 INTEGER Public,#3: timestamp_0 TIMESTAMP(3) Private), Predicate: AND(=($0, $2), <=($1, $3)), slice key: [#0: patient_id INTEGER Public, #2: patient_id0 INTEGER Public]\n"
				 + "            LogicalMerge-DistributedOblivious\n"
				 + "                JdbcTableScan-LocalClearPartitioned, schema:(#0: patient_id INTEGER Public,#1: site INTEGER Private,#2: year INTEGER Private,#3: month_id INTEGER Private,#4: visit_no INTEGER Public,#5: type_ INTEGER Private,#6: encounter_id INTEGER Private,#7: diag_src VARCHAR(65536) Protected,#8: icd9 VARCHAR(65536) Protected,#9: primary_ INTEGER Public,#10: timestamp_ TIMESTAMP(3) Private,#11: clean_icd9 VARCHAR(65536) Private,#12: major_icd9 VARCHAR(4) Protected)\n"
				 + "            LogicalMerge-DistributedOblivious\n"
				 + "                JdbcTableScan-LocalClearPartitioned, schema:(#0: patient_id INTEGER Public,#1: site INTEGER Private,#2: year INTEGER Private,#3: month_id INTEGER Private,#4: medication VARCHAR(65536) Protected,#5: dosage VARCHAR(65536) Public,#6: route VARCHAR(65536) Public,#7: timestamp_ TIMESTAMP(3) Private)\n";
	
		runTest("aspirin-count", expectedTree);
	
	}
	
	public void testCDiff() throws Exception {
		String expectedTree = "LogicalAggregate-DistributedOblivious, schema:(#0: patient_id INTEGER Public)\n"
				 + "    LogicalJoin-DistributedOblivious, schema:(#0: patient_id INTEGER Public,#1: $1 BIGINT Private,#2: $2 BIGINT Private,#3: patient_id0 INTEGER Public,#4: $10 BIGINT Private,#5: $20 BIGINT Private), Predicate: AND(=($0, $3), >=(-($4, $1), 15), <=(-($4, $1), 56), =(+($2, 1), $5)), slice key: [#0: patient_id INTEGER Public, #3: patient_id0 INTEGER Public]\n"
				 + "        LogicalWindow-DistributedOblivious, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(3) Private,#2: $2 BIGINT Private,#3: w0$o0 BIGINT Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "            LogicalMerge-DistributedOblivious\n"
				 + "                JdbcTableScan-LocalClearPartitioned, schema:(#0: patient_id INTEGER Public,#1: site INTEGER Private,#2: year INTEGER Private,#3: month_id INTEGER Private,#4: visit_no INTEGER Public,#5: type_ INTEGER Private,#6: encounter_id INTEGER Private,#7: diag_src VARCHAR(65536) Protected,#8: icd9 VARCHAR(65536) Protected,#9: primary_ INTEGER Public,#10: timestamp_ TIMESTAMP(3) Private,#11: clean_icd9 VARCHAR(65536) Private,#12: major_icd9 VARCHAR(4) Protected)\n"
				 + "        LogicalWindow-DistributedOblivious, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(3) Private,#2: $2 BIGINT Private,#3: w0$o0 BIGINT Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "            LogicalMerge-DistributedOblivious\n"
				 + "                JdbcTableScan-LocalClearPartitioned, schema:(#0: patient_id INTEGER Public,#1: site INTEGER Private,#2: year INTEGER Private,#3: month_id INTEGER Private,#4: visit_no INTEGER Public,#5: type_ INTEGER Private,#6: encounter_id INTEGER Private,#7: diag_src VARCHAR(65536) Protected,#8: icd9 VARCHAR(65536) Protected,#9: primary_ INTEGER Public,#10: timestamp_ TIMESTAMP(3) Private,#11: clean_icd9 VARCHAR(65536) Private,#12: major_icd9 VARCHAR(4) Protected)\n";
		
		runTest("cdiff", expectedTree);
	}
	
	public void testComorbidity() throws Exception {
		// TODO: in sliced case will have an extra union / merge to go from sliced to single distributed execution.
		String expectedTree = "LogicalSort-DistributedOblivious, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				 + "    LogicalAggregate-DistributedOblivious, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				 + "        LogicalMerge-DistributedOblivious\n"
				 + "            JdbcTableScan-LocalClearPartitioned, schema:(#0: patient_id INTEGER Public,#1: site INTEGER Private,#2: year INTEGER Private,#3: month_id INTEGER Private,#4: visit_no INTEGER Public,#5: type_ INTEGER Private,#6: encounter_id INTEGER Private,#7: diag_src VARCHAR(65536) Protected,#8: icd9 VARCHAR(65536) Protected,#9: primary_ INTEGER Public,#10: timestamp_ TIMESTAMP(3) Private,#11: clean_icd9 VARCHAR(65536) Private,#12: major_icd9 VARCHAR(4) Protected)\n";

	
		runTest("comorbidity", expectedTree);
	}
	

	void runTest(String testName, String expectedTree) throws Exception {
		config.resetCounters();
		
		String sql = super.readSQL(testName);
		logger.log(Level.INFO, "Parsing " + sql);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
		
		QueryCompiler qc = new QueryCompiler(secRoot);
		ExecutionStep root = qc.getRoot();
		
		String testTree = root.printTree();
		logger.log(Level.INFO, "Resolved secure tree to:\n " + testTree);
		assertEquals(expectedTree, testTree);
	}
}
