package org.smcql.compiler.emp;

import java.io.ByteArrayOutputStream;
import java.io.File;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.PumpStreamHandler;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.Utilities;

// for use in localhost setting 
public class EmpRunnable implements Runnable {
    String className;
 	int party, port;
 	boolean[] output;
 	final String smcqlRoot = Utilities.getSMCQLRoot(); // directory with pom.xml
	final String javaCppJar = System.getProperty("user.home") + "/.m2/repository/org/bytedeco/javacpp/1.4.4/javacpp-1.4.4.jar";
	final String javaCppWorkingDirectory = smcqlRoot + "/target/classes";

	
	public EmpRunnable(String aClassName, int aParty, int aPort) throws Exception {
		configure(aClassName, aParty, aPort);
	}
	
 	public void configure(String aClassName, int aParty, int aPort) throws Exception {
 		className = EmpJniUtilities.getFullyQualifiedClassName(aClassName);
 		party = aParty;
 		port = aPort;
 	}
 	
 	
    public void run(){
	    ByteArrayOutputStream stdout = new ByteArrayOutputStream(); // use this for debug info
	    ByteArrayOutputStream stderr = new ByteArrayOutputStream(); // use this to capture bitstring

    	try {
    	
    		

    	    PumpStreamHandler psh = new PumpStreamHandler(stdout, stderr);
    	    // java -cp $JAVACPP_JAR:$JAVACPP_WORKING_DIRECTORY  org.smcql.compiler.emp.generated.ClassName <party> <port> 
    	    String command = "java -cp " + javaCppJar + ":" + javaCppWorkingDirectory + " " + className + " " + party + " " + port;
    	    
    	    System.out.println("Running: " + command);
    	    
    	    CommandLine cl = CommandLine.parse(command);
    	    DefaultExecutor exec = new DefaultExecutor();
    	    exec.setStreamHandler(psh);
    	    exec.setWorkingDirectory(new File(javaCppWorkingDirectory));
    	    int exitValue = exec.execute(cl);
    	    assert(0 == exitValue);
    	    
    	    String bitString = stderr.toString(); // TODO: can we make this all happen in binary?
    	    System.out.println(stdout.toString());
    	    System.out.println("Output: " + bitString);
    	    // translate to bools
    	    output = new boolean[bitString.length()];
    	    
    	    System.out.println("Party " + party + " returned " + bitString.length() + " bits.");
    	    for(int i = 0; i < bitString.length(); ++i) {
    	    	output[i] = bitString.charAt(i) == '1' ? true : false;
    	    }
    	    
    	    
    		
    	} catch (Exception e) {
    		System.out.println("Running emp on party " + party + " failed!");
    		System.out.println("stdout: " + stdout.toString());
    		System.out.println("stderr: " + stderr.toString());

    		e.printStackTrace();
    	}
    }
    
    public boolean[] getOutput() {
    	return output;
    }
    
}
