package org.smcql.compiler.emp;

import java.net.InetAddress;
import java.util.List;
import java.util.concurrent.Callable;

import org.gridkit.nanocloud.Cloud;
import org.gridkit.nanocloud.CloudFactory;
import org.gridkit.nanocloud.RemoteNode;
import org.gridkit.nanocloud.VX;
import org.gridkit.nanocloud.telecontrol.ssh.SshSpiConf;
import org.gridkit.vicluster.ViNode;
import org.gridkit.vicluster.ViProps;
import org.gridkit.vicluster.telecontrol.ssh.RemoteNodeProps;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.util.CommandOutput;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.Utilities;

import junit.framework.TestCase;

public class EmpJniRemoteTest extends TestCase  {
	
	final String smcqlRoot = Utilities.getSMCQLRoot();
	final String className = "EmpJniDemo";
	final String localPath = "src/main/java";
	final String empCodeLocal = "org/smcql/compiler/emp/generated/" + className + ".h";
	final String jniCodeLocal = "org/smcql/compiler/emp/generated/" + className + ".java";
	
	final String fullyQualifiedClassName = "org.smcql.compiler.emp.generated." + className;
	Cloud cloud;
	String generatorHost = null;
	List<WorkerConfiguration> workers;
	final int tupleWidth = 3; //characters in string
	
	
	  protected void setUp() throws Exception {
		    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.remote";
		    System.setProperty("smcql.setup", setupFile);
			cloud = CloudFactory.createCloud();
	        RemoteNode.at(cloud.node("**")).useSimpleRemoting();

			
		    ConnectionManager cm = ConnectionManager.getInstance();
		     workers = cm.getWorkerConfigurations();
		     generatorHost = workers.get(0).hostname;
		     
		     // in case it is a hostname and not an IP address, resolve it
		     InetAddress address = InetAddress.getByName(generatorHost); 
		     generatorHost = address.getHostAddress();
		     
		     for(WorkerConfiguration w : workers) {
		    	 initializeHost(w);
		     }
	  }

	
	public void testRemoteExecution() throws Exception {
		 
		 List<boolean[]> outputs = runCloudExecution();
		 List<String> tuples = EmpJniUtilities.revealStringOutput(outputs.get(0), outputs.get(1), tupleWidth);
		 System.out.println("Output: " + tuples);
	}
	
	
	  private void initializeHost(WorkerConfiguration worker) throws Exception {
	        String host = worker.hostname;
	        String workerId = worker.workerId;

	        ViNode cloudHost = cloud.node(workerId);
	        
	        String remotePath = "/tmp/vaultdb";
	        String configPath = SystemConfiguration.getInstance().getProperty("remote-path");
	        if(configPath != null) {
	        	remotePath = configPath;
	        }

			
			
	        if(host.equalsIgnoreCase("localhost") || host.equalsIgnoreCase("127.0.0.1")) {
	                cloudHost.x(VX.TYPE).setLocal();
	                ViProps.at(cloudHost).setIsolateType(); // enable debugger  
	                cloudHost.setProp("remote.node", "false");
	        }
	        else { // remote case
	        	cloudHost.x(VX.RUNTIME).setProp("user.dir", remotePath + "/" + workerId);
	        	RemoteNodeProps.at(cloudHost).setRemoteHost(host);
	            cloudHost.setProp(SshSpiConf.SPI_JAR_CACHE, remotePath + "/" + workerId);
	            cloudHost.setProp("remote.node", "true");
	        }

	        //cloudHost.x(VX.CLASSPATH).inheritClasspath(true);
	        cloudHost.x(VX.PROCESS).addJvmArg("-Xms4096m").addJvmArg("-Xmx4g");                                                                                        
	        
	 
	    }


	  
	  
	// for remote test
	private List<boolean[]> runCloudExecution() throws Exception {
		 
		 // declare nodes
		 cloud.node("alice");
		 cloud.node("alice").setProp("party", "alice");

		 cloud.node("bob");
		 cloud.node("bob").setProp("party", "bob");

		
		 cloud.node("**").x(VX.PROCESS).addJvmArg("-Xms4096m"); // give it lots of memory
		 cloud.node("**").setProp("emp.port", SystemConfiguration.getInstance().getProperty("emp-port"));
		 
		 List<String> empLines =  Utilities.readFile(localPath + "/" + empCodeLocal);
		 cloud.node("**").setProp("emp.code", String.join("\n", empLines));
		 cloud.node("**").setProp("emp.file", empCodeLocal);
		 

		 List<String> jniLines =  Utilities.readFile(localPath + "/" + jniCodeLocal);
		 cloud.node("**").setProp("jni.code", String.join("\n", jniLines));
		 cloud.node("**").setProp("jni.file", jniCodeLocal);
		 cloud.node("**").setProp("alice.host", generatorHost);
		 
		 ViNode allNodes = cloud.node("**");
		 
		List<boolean[]> results = allNodes.massExec(new Callable<boolean[]>() {
		        
	            @Override
	            public boolean[] call() throws Exception {
	                int party = 1;
	                int port = Integer.parseInt(System.getProperty("emp.port"));
	                String empCode = System.getProperty("emp.code");
	                String jniCode = System.getProperty("jni.code");
	                String workingDirectory = System.getProperty("user.dir");	                
	                String aliceHost = System.getProperty("alice.host");
	                
	                if(System.getProperty("party").equals("bob")) {
	                	party = 2;
	                }
	            	
	                // enable for detailed info from jni 
	                //System.setProperty("org.bytedeco.javacpp.logger.debug", "true");
	              
	                	                
	                Utilities.mkdir(workingDirectory + "org/smcql/compiler/emp/generated/", workingDirectory);
      
	                String empFile = workingDirectory + "/" + empCodeLocal;
	                String jniFile = workingDirectory + "/" + jniCodeLocal;
	                
	                Utilities.writeFile(empFile, empCode);
	                Utilities.writeFile(jniFile, jniCode);
	                
	                // build it
	                EmpBuilder builder = new EmpBuilder();
	         	   	builder.compile(fullyQualifiedClassName, false);
	    
	         	   	System.out.println("Have alice at " + aliceHost);
	         	   	// run it
	         	   EmpProgram instance = (EmpProgram) builder.getClass(fullyQualifiedClassName, party, port);
	         	   instance.setGeneratorHost(aliceHost);
	         	   return instance.runProgram();
	               
	            }
	        });   
		 
		 return results;

	}
	
	
  
}
