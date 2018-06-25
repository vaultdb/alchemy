package org.smcql.executor.emp;

import org.smcql.BaseTest;
import org.smcql.codegen.CodeCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.Utilities;

public class RunnableQueryTest extends BaseTest {

	public void executeTest(String testName) throws Exception {
		SystemConfiguration.getInstance().resetCounters();
		String sql = super.readSQL(testName);
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		
		CodeCompiler cc = new CodeCompiler(secRoot);
		cc.compile(testName + ".cpp", Utilities.getCodeGenTarget() + "/" + testName);
	}
}
