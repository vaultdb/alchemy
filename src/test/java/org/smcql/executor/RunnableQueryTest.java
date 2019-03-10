package org.smcql.executor;

import java.util.ArrayList;
import java.util.List;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.EmpExecutor;
import org.smcql.plan.SecureRelRoot;

public class RunnableQueryTest extends BaseTest {
	
	public void testComorbidity() throws Exception {
		String testName = "comorbidity";
		List<String> parties = new ArrayList<String>();
		parties.add("emp");
		executeTest(testName, parties);
	}
	
	public void testCDiff() throws Exception {
		String testName = "cdiff";
		List<String> parties = new ArrayList<String>();
		parties.add("emp");
		executeTest(testName, parties);
	}
	
	public void executeTest(String testName, List<String> parties) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		
		QueryCompiler qc = new QueryCompiler(secRoot);
		EmpExecutor exec = new EmpExecutor(qc, parties);
		exec.run();
	}
}
