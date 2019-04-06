package org.smcql.executor.remote;

import org.smcql.BaseTest;
import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.RPC.RPCExecutorClient;
import org.smcql.executor.RPC.RPCExecutorServer;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.RPC.RPCExecutorUtility;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

import java.util.List;
import java.util.logging.Logger;

import static java.lang.Thread.sleep;

public class RPCExecutorServiceTest extends BaseTest {

  private RPCExecutorServer server;
  private RPCExecutorClient aliceClient;
  private RPCExecutorClient bobClient;

  protected void setUp() throws Exception {
    super.setUp();
    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
    System.setProperty("smcql.setup", setupFile);
    ConnectionManager cm = ConnectionManager.getInstance();
    server = new RPCExecutorServer();
  }

  @Override
  protected void tearDown() throws Exception {
    super.tearDown();
    server.Stop();
  }

  public void testStartServer() throws Exception {
    server.Start(
        Integer.parseInt(SystemConfiguration.getInstance().getProperty("remote-rpcservice-port")));
    server.BlockUntilShutdown();
  }

  public void testClientServer() throws Exception {
    testCaseSingle("Testing", "SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = \'414.01\'");
  }

  public void testTwoParties() throws Exception {
    server.Start(10000);
    aliceClient = new RPCExecutorClient("localhost", 10000, 1 /* party */);
    bobClient = new RPCExecutorClient("localhost", 10000, 2 /* party */);
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
    String distributedQuery =
        "WITH all_diagnoses AS ((SELECT patient_id, icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)), "
            + "all_medications AS ((SELECT patient_id FROM medications) UNION ALL (select patient_id FROM remote_medications)) "
            + "SELECT d.patient_id FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id AND icd9=\'008.45\' ORDER BY d.patient_id;";

    String aliceId = ConnectionManager.getInstance().getAlice();
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);

    QueryTable expectedOutput = SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
    ConnectionManager cm = ConnectionManager.getInstance();
    testCaseMultiParty("testTwoParties", query, expectedOutput);
  }

  // testTwoPartiesRemote tests running the query on two remote servers. It assumes that the
  // configuration  parameters for remote-alice-hostname, remote-alice-port,
  // remote-bob-hostname, remote-bob-port,  remote-empPort are set.
  // Additionally, it assumes that the RPC service is running on the host
  // "remote-alice-hostname" on the port remote-alice-port" (similarly for bob).
  // To start the RPC service on a remote machine run:
  // mvn -Dtest=org.smcql.executor.remote.RPCExecutorServiceTest#testStartServer test
  // This starts an RPCExecutorServer at port remote-rpcservice-port (set in configuration).
  public void testTwoPartiesRemote() throws Exception {
    String query =
        "SELECT  d.patient_id FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id WHERE icd9=\'008.45\'";
    String distributedQuery =
        "WITH all_diagnoses AS ((SELECT patient_id, icd9 FROM diagnoses) UNION ALL (SELECT patient_id, icd9 FROM remote_diagnoses)), "
            + "all_medications AS ((SELECT patient_id FROM medications) UNION ALL (select patient_id FROM remote_medications)) "
            + "SELECT d.patient_id FROM all_diagnoses d JOIN all_medications m ON d.patient_id = m.patient_id AND icd9=\'008.45\' ORDER BY d.patient_id;";
    String aliceId = ConnectionManager.getInstance().getAlice();
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);

    QueryTable expectedOutput = SqlQueryExecutor.query(distributedQuery, outSchema, aliceId);
    QueryTable output = RPCExecutorUtility.remoteQuery("testTwoParties", query);
    output.sort();
    assertEquals(output, expectedOutput);
  }

  protected void testCaseSingle(String testName, String sql) throws Exception {
    EmpJniUtilities.cleanEmpCode(testName);
    SystemConfiguration.getInstance().resetCounters();
    Logger logger = SystemConfiguration.getInstance().getLogger();

    SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
    QueryCompiler qc = new QueryCompiler(secRoot);
    qc.writeOutEmpFile();
    String jniFile = testName + ".java";
    String empFile = testName + ".h";
    List<String> jniText = FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + jniFile);
    List<String> empText = FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + empFile);
    boolean[] aliceOutput =
        aliceClient.executeSegment(
            testName,
            String.join("\n", jniText),
            String.join("\n", empText),
            jniFile,
            empFile,
            45454 /* empPort */);
  }

  // This is a very hacky test case to run on a single machine.
  protected void testCaseMultiParty(String testName, String sql, QueryTable expectedOutput)
      throws Exception {
    EmpJniUtilities.cleanEmpCode(testName);
    SystemConfiguration.getInstance().resetCounters();
    Logger logger = SystemConfiguration.getInstance().getLogger();

    String testNameAlice = testName + "alice";
    String testNameBob = testName + "bob";
    SecureRelRoot secRoot = new SecureRelRoot(testNameAlice, sql);
    QueryCompiler qc = new QueryCompiler(secRoot);
    qc.writeOutEmpFile();

    SecureRelRoot secRootBob = new SecureRelRoot(testNameBob, sql);
    QueryCompiler qcBob = new QueryCompiler(secRootBob);
    qcBob.writeOutEmpFile();

    String jniFileAlice = testNameAlice + ".java";
    String empFileAlice = testNameAlice + ".h";
    List<String> jniTextAlice =
        FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + jniFileAlice);
    List<String> empTextAlice =
        FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + empFileAlice);

    String jniFileBob = testNameBob + ".java";
    String empFileBob = testNameBob + ".h";
    List<String> jniTextBob = FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + jniFileBob);
    List<String> empTextBob = FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + empFileBob);

    boolean[] aliceOutput;
    boolean[] bobOutput;
    Thread t1 =
        new Thread(
            () ->
                aliceClient.executeSegment(
                    testNameAlice,
                    String.join("\n", jniTextAlice),
                    String.join("\n", empTextAlice),
                    jniFileAlice,
                    empFileAlice,
                    45454 /* empPort */));
    t1.start();
    sleep(3000);
    Thread t2 =
        new Thread(
            () ->
                bobClient.executeSegment(
                    testNameBob,
                    String.join("\n", jniTextBob),
                    String.join("\n", empTextBob),
                    jniFileBob,
                    empFileBob,
                    45454 /* empPort */));
    t2.start();
    t1.join();
    t2.join();

    aliceOutput = aliceClient.getLastOutput();
    bobOutput = bobClient.getLastOutput();
    boolean[] decrypted = EmpJniUtilities.decrypt(aliceOutput, bobOutput);
    QueryTable qt = new QueryTable(decrypted, qc.getOutSchema());
    qt.sort();
    assertEquals(qt, expectedOutput);
  }
}
