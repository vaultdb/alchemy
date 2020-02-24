package org.vaultdb.executor.emp.stub;

import org.vaultdb.BaseTest;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpRunnable;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.util.EmpJniUtilities;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

public class EmpJniJdbcLocalTest extends BaseTest {
  final String fullyQualifiedClassName = "org.vaultdb.compiler.emp.generated.EmpJniJdbcDemo";
  final int tupleWidth = 3; // characters in string

  protected void setUp() throws Exception {

    super.setUp();
  }

  public void testEmpJni() throws Exception {
    int empPort = EmpJniUtilities.getEmpPort();

    EmpRunnable aliceRunnable = new EmpRunnable(fullyQualifiedClassName, 1, empPort);
    EmpRunnable bobRunnable = new EmpRunnable(fullyQualifiedClassName, 2, empPort);

    EmpBuilder builder = new EmpBuilder(fullyQualifiedClassName);
    builder.compile();

    Thread alice = new Thread(aliceRunnable);
    alice.start();

    Thread bob = new Thread(bobRunnable);
    bob.start();

    alice.join();
    bob.join();

    String aliceOutput = aliceRunnable.getOutputString();
    DBQueryProtos.Table aliceTable = aliceRunnable.getOutputProtoTable();
    // DBQueryProtos.Table aliceTable = DBQueryProtos.Table.parseFrom(aliceOutput.getBytes());
    String bobOutput = bobRunnable.getOutputString();
    DBQueryProtos.Table bobTable = bobRunnable.getOutputProtoTable();
    System.out.println(bobTable.getRowCount());

    logger.info(
        "Alice output len = "
            + aliceOutput.getBytes().length
            + ", bob's is "
            + bobOutput.getBytes().length);

    List<Long> output = EmpJniUtilities.revealTableOutput(aliceTable, bobTable);
    logger.info("Query output: " + output);

    long[] arr = {1, 2, 3, 4, 1, 2, 3, 4, 2, 1, 5, 6, 1, 2, 5, 6};
    List<Long> expectedOutput = Arrays.stream(arr).boxed().collect(Collectors.toList());

    assertEquals(expectedOutput, output);
  }
}
