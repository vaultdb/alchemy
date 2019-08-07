package org.vaultdb.executor.emp.stub;

import java.net.InetAddress;
import java.util.BitSet;
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
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpProgram;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

import junit.framework.TestCase;


//  Minimal EMP test, based on emp-jni repo
public class EmpJniRemoteTest extends TestCase  {
	
	final String smcqlRoot = Utilities.getSMCQLRoot();
	final String className = "EmpJniDemo";
	
	final String fullyQualifiedClassName = "org.smcql.compiler.emp.generated." + className;
	Cloud cloud;
	String generatorHost = null;
	List<WorkerConfiguration> workers;
	final int tupleWidth = 3; //characters in string
	SystemConfiguration config;
	
	  protected void setUp() throws Exception {

		  // TODO: test in remote setting by setting line in setup.global
		  // "distributed-eval-enabled=true"
		  
		  System.setProperty("smcql.location", "distributed");
			
		  config  = SystemConfiguration.getInstance(); // initialize config
		    
		    if(config.getProperty("location").equals("distributed"))
		    	initializeCloud();
	  }
	  
	  
	  private void initializeCloud() throws Exception {
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

			 // declare nodes
			 cloud.node("alice");
			 cloud.node("alice").setProp("party", "alice");

			 cloud.node("bob");
			 cloud.node("bob").setProp("party", "bob");

			
			 cloud.node("**").x(VX.PROCESS).addJvmArg("-Xms4096m"); // give it lots of memory
			 cloud.node("**").setProp("emp.port", SystemConfiguration.getInstance().getProperty("emp-port"));
			 cloud.node("**").setProp("class.name", className);
			 
			 String codeGenTarget = Utilities.getCodeGenTarget();
			 List<String> empLines =  FileUtilities.readFile(codeGenTarget + "/" + className + ".h");
			 cloud.node("**").setProp("emp.code", String.join("\n", empLines));
			 

			 List<String> jniLines =  FileUtilities.readFile(codeGenTarget + "/" + className + ".java");
			 cloud.node("**").setProp("jni.code", String.join("\n", jniLines));
			 cloud.node("**").setProp("alice.host", generatorHost);

		     cloud.node("**").setProp("smcql.setup.str", SystemConfiguration.getInstance().getSetupParameters());
		     cloud.node("**").setProp("emp.port", String.valueOf(EmpJniUtilities.getEmpPort()));

		  
	  }

	
	public void testRemoteExecution() throws Exception {
		 
	    if(config.getProperty("location").equals("distributed")) {
	    	List<String> outputs = runCloudExecution();
	    	List<String> tuples = EmpJniUtilities.revealStringOutput(outputs.get(0), outputs.get(1), tupleWidth);
	    	System.out.println("Output: " + tuples);
	    }
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


	  
	  
	private List<String> runCloudExecution() throws Exception {
		 

		 ViNode allNodes = cloud.node("**");
		 
		List<String> results = allNodes.massExec(new Callable<String>() {
		        
	            @Override
	            public String call() throws Exception {
	                int party = 1;
	                int port = Integer.parseInt(System.getProperty("emp.port"));
	                String empCode = System.getProperty("emp.code");
	                String jniCode = System.getProperty("jni.code");
	                String workingDirectory = System.getProperty("user.dir");	                
	                String aliceHost = System.getProperty("alice.host");
	                SystemConfiguration config = SystemConfiguration.getInstance();
	                config.setProperty("node-type", "remote");
	                String codeGenTarget = Utilities.getCodeGenTarget();
	                
	                if(System.getProperty("party").equals("bob")) {
	                	party = 2;
	                }
	            	
	                // enable for detailed info from jni 
	                //System.setProperty("org.bytedeco.javacpp.logger.debug", "true");
	              
	                	                
	                Utilities.mkdir(workingDirectory + "/" + codeGenTarget, workingDirectory);
	                
	                String empFile = workingDirectory + "/" + className + ".h";
	                String jniFile = workingDirectory + "/" + className + ".java";
	                
	                FileUtilities.writeFile(empFile, empCode);
	                FileUtilities.writeFile(jniFile, jniCode);
	                
	                // build it
	                EmpBuilder builder = new EmpBuilder(fullyQualifiedClassName);
	         	   	builder.compile();
	    
	         	   	// run it
	         	   EmpProgram instance = (EmpProgram) builder.getClass(party, port);
	         	   instance.setGeneratorHost(aliceHost);
	         	   instance.runProgram();
	         	   return instance.getOutputString();
	               
	            }
	        });   
		 
		 return results;

	}
	
	
  
}
