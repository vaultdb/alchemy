package org.vaultdb.runner;

import org.apache.calcite.sql.SqlDialect;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.executor.EmpExecutor;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.util.Utilities;

public class VaultDBRunner {
	protected SqlDialect dialect = SqlDialect.DatabaseProduct.POSTGRESQL.getDialect();
	protected String codePath = Utilities.getVaultDBRoot() + "/conf/workload/sql";
	protected static WorkerConfiguration honestBroker;
	protected static SqlStatementParser parser;
	
	private static void setUp() throws Exception {
		System.setProperty("smcql.setup", Utilities.getVaultDBRoot() + "/conf/setup.remote");

		parser = new SqlStatementParser();
		honestBroker = SystemConfiguration.getInstance().getHonestBrokerConfig();
		SystemConfiguration.getInstance().resetCounters();
	}
	
	public static void main(String[] args) throws Exception {
		setUp();

		String sql = args[0];
		System.out.println("\nQuery:\n" + sql);
		
		String aWorkerId = args[1];
		String bWorkerId = args[2];
		
		String testName = "userQuery";
		SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		QueryCompiler qc = new QueryCompiler(secRoot, sql);
		
		EmpExecutor exec = new EmpExecutor(qc, aWorkerId, bWorkerId);
		exec.run();
		
	    QueryTable results = exec.getOutput();
	    System.out.println("\nOutput:\n" + results);
	    System.exit(0);
	}

}
