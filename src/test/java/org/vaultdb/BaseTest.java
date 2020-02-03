package org.vaultdb;

import java.io.IOException;
import java.util.logging.Logger;

import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlNode;
import org.vaultdb.codegen.QueryCompiler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.executor.EmpExecutor;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.plan.SecureRelRoot;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

import junit.framework.TestCase;


public abstract class BaseTest extends TestCase {
	protected SqlStatementParser parser;
	protected SqlNode root;
	protected RelRoot relRoot;
	protected SqlDialect dialect;
	protected String codePath = Utilities.getVaultDBRoot() + "/conf/workload/sql";
	protected WorkerConfiguration honestBroker;
	protected Logger logger;
	protected SystemConfiguration config;
	protected SystemCatalog catalog;
	
	

	protected void setUp() throws Exception {
	
		System.setProperty("vaultdb.setup", Utilities.getVaultDBRoot() + "/conf/setup.global");
		SystemConfiguration.resetConfiguration();
		SystemCatalog.resetInstance();
		ConnectionManager.reset();

		
		config = SystemConfiguration.getInstance();
		catalog = SystemCatalog.getInstance();
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
	  
	  @Override
	  protected void tearDown() throws Exception {

		  // clean up any dangling resources
		  ConnectionManager connections = ConnectionManager.getInstance();
		  if(connections != null) {
			  connections.closeConnections();
			  ConnectionManager.reset();
		  }
		  
		  SystemConfiguration.getInstance().closeCalciteConnection();
		  SystemConfiguration.resetConfiguration();
		  SystemCatalog.resetInstance();
		  
		  // delete any generated classfiles
		  String classFiles = Utilities.getVaultDBRoot() + "/target/classes/org/vaultdb/compiler/emp/generated/*.class";
		  Utilities.runCmd("rm " + classFiles);
		  
		  
		  
	  }
}
