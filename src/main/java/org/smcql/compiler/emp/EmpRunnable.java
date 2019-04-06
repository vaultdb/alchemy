package org.smcql.compiler.emp;

import java.io.ByteArrayOutputStream;
import java.io.File;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.PumpStreamHandler;
import org.gridkit.internal.com.jcraft.jsch.Logger;
import org.smcql.config.SystemConfiguration;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

// for use in localhost setting
public class EmpRunnable implements Runnable {
  String className;
  int party, port;
  boolean[] output;
  String outputString;
  final String smcqlRoot = Utilities.getSMCQLRoot(); // directory with pom.xml
  // TODO: adjust to versions in pom.xml
  final String javaCppJar =
      System.getProperty("user.home")
          + "/.m2/repository/org/bytedeco/javacpp/1.4.4/javacpp-1.4.4.jar";
  final String calciteCoreJar =
      System.getProperty("user.home")
          + ".m2/repository/org/apache/calcite/calcite-core/1.18.0/calcite-core-1.18.0.jar";
  final String javaCppWorkingDirectory = smcqlRoot + "/target/classes";

  public EmpRunnable(String aClassName, int aParty, int aPort) throws Exception {
    configure(aClassName, aParty, aPort);
  }

  public void configure(String aClassName, int aParty, int aPort) throws Exception {
    className = EmpJniUtilities.getFullyQualifiedClassName(aClassName);
    party = aParty;
    port = aPort;
  }

  public void run() {
    ByteArrayOutputStream stdout = new ByteArrayOutputStream(); // use this for debug info
    ByteArrayOutputStream stderr = new ByteArrayOutputStream(); // use this to capture bitstring

    try {

      java.util.logging.Logger logger = SystemConfiguration.getInstance().getLogger();

      PumpStreamHandler psh = new PumpStreamHandler(stdout, stderr);
      String classPath = System.getProperty("java.class.path");
      classPath = classPath.replace(" ", "\\ ");
      // java -cp $JAVACPP_JAR:$JAVACPP_WORKING_DIRECTORY
      // org.smcql.compiler.emp.generated.ClassName <party> <port>
      String command = "java -cp " + classPath + " " + className + " " + party + " " + port;

      String outFile = "run-alice.sh";
      if (party == 2) {
        outFile = "run-bob.sh";
      }

      // save it for later for debugging
      FileUtils.writeFile(outFile, command);

      CommandLine cmdl = new CommandLine("java");
      cmdl.addArgument("-cp");
      cmdl.addArgument(classPath);
      cmdl.addArgument(className);
      cmdl.addArgument(String.valueOf(party));
      cmdl.addArgument(String.valueOf(port));

      // CommandLine cl = CommandLine.parse(command);
      DefaultExecutor exec = new DefaultExecutor();
      exec.setStreamHandler(psh);
      exec.setWorkingDirectory(new File(javaCppWorkingDirectory));
      System.out.println("running command: " + command);
      int exitValue = exec.execute(cmdl);
      assert (0 == exitValue);

      String bitString = stderr.toString(); // TODO: can we make this all happen in binary?
      outputString = bitString;
      logger.fine("Output: " + bitString);
      logger.info("Party " + party + " returned " + bitString.length() + " bits.");
      // translate to bools
      output = stringToBool(bitString);
      logger.info("stdout: " + stdout.toString()); // uncomment to show cout statements

    } catch (Exception e) {
      System.err.println("Running emp on party " + party + " failed!");
      System.err.println("stdout: " + stdout.toString());
      System.err.println("stderr: " + stderr.toString());

      e.printStackTrace();
    }
  }

  public static boolean[] stringToBool(String s) {
    boolean[] b = new boolean[s.length()];

    for (int i = 0; i < s.length(); ++i) {
      b[i] = (s.charAt(i) == '1') ? true : false;
    }
    return b;
  }

  public String getOutputString() {
    return outputString;
  }

  public boolean[] getOutput() {
    return output;
  }
}
