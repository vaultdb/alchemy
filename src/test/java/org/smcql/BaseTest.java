package org.smcql;

import java.io.IOException;
import java.util.logging.Logger;

import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlNode;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.db.schema.SystemCatalog;
import org.smcql.executor.EmpExecutor;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.parser.SqlStatementParser;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.FileUtilities;
import org.smcql.util.Utilities;

import junit.framework.TestCase;


public abstract class BaseTest extends TestCase {
	protected SqlStatementParser parser;
	protected SqlNode root;
	protected RelRoot relRoot;
	protected SqlDialect dialect;
	protected String codePath = Utilities.getSMCQLRoot() + "/conf/workload/sql";
	protected WorkerConfiguration honestBroker;
	protected Logger logger;
	protected SystemConfiguration config;
	
	

	protected void setUp() throws Exception {
	
		System.setProperty("smcql.setup", Utilities.getSMCQLRoot() + "/conf/setup.global");
		SystemConfiguration.resetConfiguration();
		SystemCatalog.resetInstance();
		
		config = SystemConfiguration.getInstance();
		parser = new SqlStatementParser();
		honestBroker = SystemConfiguration.getInstance().getHonestBrokerConfig();
		logger = SystemConfiguration.getInstance().getLogger();
		dialect = config.DIALECT;
		
	}

	
	
	
	
	protected String readSQL(String testName) throws IOException {
		String fileName = codePath + "/" + testName + ".sql";
		String sql = FileUtilities.readSQL(fileName);
		return sql;

	}
	
	


	  protected QueryTable getExpectedOutput(String query) throws Exception {

		    String unionedId = ConnectionManager.getInstance().getUnioned();
		    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		    return SqlQueryExecutor.query(query, outSchema, unionedId);
		    
		  }

	  
	  protected void testQuery(String testName, String sql) throws Exception {
		 
		  	SystemConfiguration.getInstance().resetCounters();
		    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);

		    System.out.println("Initial schema: " + secRoot.getPlanRoot().getSchema() );
		    QueryCompiler qc = new QueryCompiler(secRoot);
		    qc.writeOutEmpFile();

		    String empTarget = Utilities.getCodeGenTarget() + "/" + testName + ".h";
		    String jniTarget = Utilities.getCodeGenTarget() + "/" + testName + ".java";

		    assertTrue(FileUtilities.fileExists(empTarget));
		    assertTrue(FileUtilities.fileExists(jniTarget));

		    EmpExecutor exec = new EmpExecutor(qc);
		    exec.run();

		    QueryTable expectedOutput = getExpectedOutput(sql);
		    QueryTable observedOutput = exec.getOutput();

		    logger.info("Observed output: \n" + observedOutput);
		    logger.info("Expected output: \n" + expectedOutput);

		    assertEquals(expectedOutput.tupleCount(), observedOutput.tupleCount());
		    assertEquals(expectedOutput, observedOutput);

		  }
}
