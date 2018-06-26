package org.smcql.executor.emp;

import java.util.ArrayList;
import java.util.List;

import org.smcql.BaseTest;
import org.smcql.codegen.CodeCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.EMPQueryExecutor;
import org.smcql.plan.SecureRelRoot;

public class RunnableQueryTest extends BaseTest {
	public String aWorkerId = "emp";
	public String bWorkerId = "emp";

	public void testComorbidity() throws Exception {
		String testName = "comorbidity";
		executeTest(testName);
	}
	
	public void executeTest(String testName) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		
		CodeCompiler cc = new CodeCompiler(secRoot);
		String empCode = cc.compile();
		List<String> parties = new ArrayList<String>();
		parties.add(aWorkerId);
		parties.add(bWorkerId);
		
		EMPQueryExecutor exec = new EMPQueryExecutor(empCode, parties);
		exec.run();
	}
}
