package org.vaultdb.compiler.emp;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.BitSet;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.PumpStreamHandler;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

// for use in localhost setting
public class EmpRunnable implements Runnable {
  String className;
  int party, port;
  BitSet output;
  String outputString;
  
  int outputSize = 0;
  
  final String smcqlRoot = Utilities.getVaultDBRoot(); // directory with pom.xml
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
      FileUtilities.writeFile(outFile, command);

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
      bitString = bitString.substring(bitString.lastIndexOf("\n") + 1);
      outputString = bitString;
      //logger.info("Output: " + bitString);
      logger.info("Party " + party + " returned " + bitString.length() + " bits.");
  
      // translate to bools
      output = EmpJniUtilities.stringToBitSet(bitString);
      logger.info("stdout: " + stdout.toString()); 

    } catch (Exception e) {
      System.err.println("Running emp on party " + party + " failed!");
      System.err.println("stdout: " + stdout.toString());
      System.err.println("stderr: " + stderr.toString());

      e.printStackTrace();
    }
  }

 
  public String getOutputString() {
    return outputString;
  }

  public BitSet getOutput() {
    return output;
  }
  
  public int getOutputLength() {
	 	if(outputString == null)
    		return 0;
    	
    	return outputString.length();
    }
}
