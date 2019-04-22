package org.smcql.executor.RPC;

import org.smcql.codegen.QueryCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.plan.SecureRelRoot;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

import java.util.Arrays;
import java.util.logging.Logger;

import static java.lang.Thread.sleep;

public class RPCExecutorUtility {

  public static QueryTable remoteQuery(String queryName, String sql) throws Exception {
    return remoteQuery(
        queryName,
        sql,
        SystemConfiguration.getInstance().getProperty("remote-alice-hostname"),
        SystemConfiguration.getInstance().getProperty("remote-bob-hostname"),
        Integer.parseInt(SystemConfiguration.getInstance().getProperty("remote-alice-port")),
        Integer.parseInt(SystemConfiguration.getInstance().getProperty("remote-bob-port")));
  }

  public static QueryTable remoteQuery(
      String queryName,
      String sql,
      String aliceHostname,
      String bobHostName,
      int alicePort,
      int bobPort)
      throws Exception {
    RPCExecutorClient aliceClient =
        new RPCExecutorClient(aliceHostname, alicePort, 1 /* party, alice is party 1 */);
    RPCExecutorClient bobClient =
        new RPCExecutorClient(bobHostName, bobPort, 2 /* party, bob is party 1 */);

    EmpJniUtilities.cleanEmpCode(queryName);
    SystemConfiguration.getInstance().resetCounters();
    ConnectionManager cm = ConnectionManager.getInstance();
    cm.getWorker(cm.getAlice()).hostname = aliceHostname;
    System.out.println(aliceHostname);
    Logger logger = SystemConfiguration.getInstance().getLogger();
    SecureRelRoot secRoot = new SecureRelRoot(queryName, sql);
    QueryCompiler qc = new QueryCompiler(secRoot);
    qc.writeOutEmpFile();
    String jniFile = queryName + ".java";
    String empFile = queryName + ".h";
    String jniSource =
        String.join("\n", FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + jniFile));
    String empSource =
        String.join("\n", FileUtils.readFile(Utilities.getCodeGenTarget() + "/" + empFile));
    int empPort = Integer.parseInt(SystemConfiguration.getInstance().getProperty("remote-empPort"));
    Thread aliceThread =
        new Thread(
            () ->
                aliceClient.executeSegment(
                    queryName, jniSource, empSource, jniFile, empFile, empPort));

    Thread bobThread =
        new Thread(
            () ->
                bobClient.executeSegment(
                    queryName, jniSource, empSource, jniFile, empFile, empPort));
    aliceThread.start();
    bobThread.start();
    aliceThread.join();
    bobThread.join();
    
    boolean[] aliceOutput = aliceClient.getLastOutput();
    boolean[] bobOutput = bobClient.getLastOutput();
    boolean[] decrypted = EmpJniUtilities.decrypt(aliceOutput, bobOutput);

    // solve for bit size    
    int tupleLen = qc.getOutSchema().size() + 1;  // +1 for the dummy tag
    int tupleCount = decrypted.length / tupleLen;
    
    // TODO: Keith figure out how to do this with pointer arithmetic
    boolean[] dummyTags = Arrays.copyOfRange(decrypted, 0, tupleCount);
    boolean[] tuples = Arrays.copyOfRange(decrypted, tupleCount, decrypted.length);

    return new QueryTable(dummyTags, tuples, qc.getOutSchema());
  }
}
