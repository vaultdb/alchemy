package org.smcql.planner;

import java.io.IOException;
import java.sql.SQLException;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.sql.SqlExplainLevel;
import org.apache.calcite.sql.parser.SqlParseException;
import org.apache.calcite.tools.RelConversionException;
import org.apache.calcite.tools.ValidationException;
import org.apache.log4j.lf5.LogLevel;
import org.smcql.BaseTest;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelRoot;
import org.smcql.plan.operator.Operator;

public class TreeBuilderTest extends BaseTest {

	public void testAspirinCount() throws Exception {
		String expectedTree = "LogicalAggregate-Secure, schema:(#0: rx_cnt BIGINT Public)\n"
				 + "    LogicalAggregate-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "        LogicalProject-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "            LogicalJoin-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: patient_id0 INTEGER Public,#3: timestamp_0 TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public, #2: patient_id0 INTEGER Public]\n"
				 + "                LogicalProject-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                    LogicalFilter-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: icd9 VARCHAR(7) Protected), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                        LogicalProject-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: icd9 VARCHAR(7) Protected)\n"
				 + "                            JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private)\n"
				 + "                LogicalProject-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                    LogicalFilter-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: medication VARCHAR(2147483647) Protected), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                        LogicalProject-Plain, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: medication VARCHAR(2147483647) Protected)\n"
				 + "                            JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: medication VARCHAR(2147483647) Protected,#4: dosage VARCHAR(2147483647) Public,#5: route VARCHAR(2147483647) Public,#6: site INTEGER Private)\n";

		runTest("aspirin-count", expectedTree);
	
	}
	
	//TODO: get rid of adjacent projects
	// easier to reason about if we move it down to above scan
	public void testCDiff() throws Exception {
		String expectedTree = "LogicalAggregate-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "    LogicalProject-Slice, schema:(#0: patient_id INTEGER Public), slice key: [#0: patient_id INTEGER Public]\n"
				 + "        LogicalJoin-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private,#3: patient_id0 INTEGER Public,#4: timestamp_0 TIMESTAMP(6) Private,#5: w0$o00 INTEGER Private), slice key: [#0: patient_id INTEGER Public, #3: patient_id0 INTEGER Public]\n"
				 + "            LogicalWindow-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                LogicalProject-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                    LogicalFilter-Slice, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                        JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private)\n"
				 + "            LogicalWindow-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private,#2: w0$o0 INTEGER Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                LogicalProject-Slice, schema:(#0: patient_id INTEGER Public,#1: timestamp_ TIMESTAMP(6) Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                    LogicalFilter-Slice, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private), slice key: [#0: patient_id INTEGER Public]\n"
				 + "                        JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private)\n";
		runTest("cdiff", expectedTree);
	}
	
	public void testComorbidity() throws Exception {
		
		String expectedTree = "LogicalSort-Secure, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				 + "    LogicalAggregate-Secure, schema:(#0: major_icd9 VARCHAR(4) Protected,#1: cnt BIGINT Protected)\n"
				 + "        LogicalProject-Plain, schema:(#0: major_icd9 VARCHAR(4) Protected)\n"
				 + "            JdbcTableScan-Plain, schema:(#0: patient_id INTEGER Public,#1: year_ INTEGER Private,#2: timestamp_ TIMESTAMP(6) Private,#3: visit_no INTEGER Public,#4: type_ INTEGER Private,#5: encounter_id INTEGER Private,#6: diag_src VARCHAR(30) Protected,#7: icd9 VARCHAR(7) Protected,#8: major_icd9 VARCHAR(4) Protected,#9: primary_ INTEGER Public,#10: site INTEGER Private)\n";
		
		runTest("comorbidity", expectedTree);
	}
	

	void runTest(String testName, String expectedTree) throws Exception {
		Logger logger = SystemConfiguration.getInstance().getLogger();

		String sql = super.readSQL(testName);
		logger.log(Level.INFO, "Parsing " + sql);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
	
		logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
		
		String testTree = secRoot.toString();
		logger.log(Level.INFO, "Resolved tree to:\n " + testTree);
		assertEquals(expectedTree, testTree);
	}
}
