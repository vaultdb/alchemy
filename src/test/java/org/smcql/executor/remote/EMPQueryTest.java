package org.smcql.executor.remote;

import java.util.ArrayList;
import java.util.List;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelRoot;

public class EMPQueryTest extends BaseTest {
	public void testComorbidity() throws Exception {
		String testName = "comorbidity";
		List<String> parties = new ArrayList<String>();
		parties.add("codd03");
		parties.add("codd04");
		executeTest(testName, parties);
	}
	
	public void testCDiff() throws Exception {
		String testName = "cdiff";
		List<String> parties = new ArrayList<String>();
		parties.add("codd03");
		parties.add("codd04");
		executeTest(testName, parties);
	}
	
	public void executeTest(String testName, List<String> parties) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		QueryCompiler qc = new QueryCompiler(secRoot);
		/*
		String empCode = cc.compile();
		System.out.println(empCode);
		cc.writeToDisk("smcql.cpp", Utilities.getSMCQLRoot()+ "/bin/");
		EMPQueryExecutor exec = new EMPQueryExecutor(empCode, parties);
		exec.run();*/
	}
}
