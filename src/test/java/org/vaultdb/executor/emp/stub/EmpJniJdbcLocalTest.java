package org.vaultdb.executor.emp.stub;

import org.vaultdb.BaseTest;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpRunnable;
import org.vaultdb.util.EmpJniUtilities;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

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
    String bobOutput = bobRunnable.getOutputString();

    logger.info("Alice output len = " + aliceOutput.length() + ", bob's is " + bobOutput.length());

    List<String> output = EmpJniUtilities.revealStringOutput(aliceOutput, bobOutput, tupleWidth);
    logger.info("Query output: " + output);

    List<String> expectedOutput = new ArrayList<String>(Arrays.asList("008", "414", "008", "414"));

    assertEquals(expectedOutput, output);
  }
}
