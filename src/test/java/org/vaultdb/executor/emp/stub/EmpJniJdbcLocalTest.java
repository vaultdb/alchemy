package org.vaultdb.executor.emp.stub;

import org.vaultdb.BaseTest;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpRunnableQueryTable;
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

  public void testEmpJniJdbc() throws Exception {
    int empPort = EmpJniUtilities.getEmpPort();

    EmpRunnableQueryTable aliceRunnable = new EmpRunnableQueryTable(fullyQualifiedClassName, 1, empPort);
    EmpRunnableQueryTable bobRunnable = new EmpRunnableQueryTable(fullyQualifiedClassName, 2, empPort);

    EmpBuilder builder = new EmpBuilder(fullyQualifiedClassName);
    builder.compile();

    Thread alice = new Thread(aliceRunnable);
    alice.start();

    Thread bob = new Thread(bobRunnable);
    bob.start();

    alice.join();
    bob.join();

    DBQueryProtos.Table aliceTable = aliceRunnable.getOutputProtoTable();
    DBQueryProtos.Table bobTable = bobRunnable.getOutputProtoTable();

    List<Long> output = EmpJniUtilities.revealTableOutput(aliceTable, bobTable);
    logger.info("Query output: " + output);

    long[] arr = {1, 2, 3, 4, 1, 2, 3, 4, 2, 1, 5, 6, 1, 2, 5, 6};
    List<Long> expectedOutput = Arrays.stream(arr).boxed().collect(Collectors.toList());

    assertEquals(expectedOutput, output);
  }
}
