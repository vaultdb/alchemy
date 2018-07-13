package org.smcql.executor;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.ExecuteException;
import org.apache.commons.exec.PumpStreamHandler;
import org.apache.commons.lang3.StringUtils;
import org.gridkit.nanocloud.Cloud;
import org.gridkit.nanocloud.CloudFactory;
import org.gridkit.nanocloud.RemoteNode;
import org.gridkit.nanocloud.telecontrol.ssh.SshSpiConf;
import org.gridkit.vicluster.ViNode;
import org.gridkit.vicluster.telecontrol.Classpath;
import org.gridkit.vicluster.telecontrol.ssh.RemoteNodeProps;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.smc.SecureBufferPool;
import org.smcql.util.Utilities;


public class EMPQueryExecutor implements Runnable {
	String empCode;
	String remotePath;
	Cloud cloud;
	
	public EMPQueryExecutor(String empCode, List<String> parties) throws Exception {
		if (parties.size() > 2) 
			throw new Exception("SMCQL does not support execution for more than two parties!");
		
		this.empCode = empCode;		
		List<WorkerConfiguration> workers = new ArrayList<WorkerConfiguration>();
		
		for (String workerId : parties) 
			workers.add(ConnectionManager.getInstance().getWorker(workerId));
		
		remotePath = SystemConfiguration.getInstance().getProperty("remote-path");
		if(remotePath == null)
			remotePath = "/tmp/smcql";
		
		prepareWorkers(workers);
	}
	
	private void prepareWorkers(List<WorkerConfiguration> workers) throws Exception {
		cloud = CloudFactory.createCloud();
		RemoteNode.at(cloud.node("**")).useSimpleRemoting();
		
		for (WorkerConfiguration w : workers) {
			System.out.println("Executing query on worker: " + w);
			initializeHost(w);	
		}
		
		String bufferPoolPointers = SecureBufferPool.getInstance().getPointers();
		
		cloud.node("**").setProp("plan.pointers", bufferPoolPointers);
		cloud.node("**").setProp("smcql.setup.str", getSetupParameters());
		cloud.node("**").setProp("smcql.connections.str", getConnectionParameters());
		cloud.node("**").setProp("smcql.root", "");
		
		cloud.node("**").touch();
		cloud.node("**").massExec(new Callable<Void>() {

            @Override
            public Void call() throws Exception {
            	String pointers = System.getProperty("plan.pointers");
            	SecureBufferPool.getInstance().addPointers(pointers);
            	return null;
            	
            }
        });
	}
	
	private void initializeHost(WorkerConfiguration worker) throws Exception {
		String host = worker.hostname;
		String workerId = worker.workerId;
		
		ViNode cloudHost = cloud.node(workerId);		
		RemoteNodeProps.at(cloudHost).setRemoteHost(host);
		cloudHost.setProp(SshSpiConf.SPI_JAR_CACHE, remotePath);
		cloud.node(worker.workerId).setProp("workerId", worker.workerId);
	}
	
	private String getSetupParameters() throws Exception {
		String srcFile = SystemConfiguration.getInstance().getConfigFile();
		List<String> params = Utilities.readFile(srcFile);
		return StringUtils.join(params.toArray(), '\n');	
	}
	
	private String getConnectionParameters() throws Exception {
		String srcFile = SystemConfiguration.getInstance().getProperty("data-providers");
		List<String> params = Utilities.readFile(srcFile);
		return StringUtils.join(params.toArray(), '\n');
	}
	
	public static String getRemotePathToJar(final String partOfJarName) throws Exception {
		 for (Classpath.ClasspathEntry cpe : Classpath.getClasspath(ClassLoader.getSystemClassLoader())) {
			 if (cpe.getFileName().contains(partOfJarName)) {
				 return File.separatorChar + cpe.getContentHash() + File.separatorChar + cpe.getFileName();
             }
         }
         throw new Exception("Jar not found!");
    }
	
	private boolean copyFile(String source, String destination) {
		CommandLine l = CommandLine.parse("scp " + source + " " + destination);
		DefaultExecutor e = new DefaultExecutor();
		try {
			e.execute(l);
		} catch (Exception ex) {
			System.out.println("Failed to copy file");
			return false;
		}
		return true;
	}
	
	//TODO: Remove hard-coding
	@Override
	public void run() {
		//String code = this.empCode;
		boolean success = copyFile(Utilities.getSMCQLRoot() + "/bin/smcql.cpp", "johesbater@ubuntu:~/Projects/emp/empsql/test");
		if (!success)
			return;
		
		List<String> results = cloud.node("**").massExec(new Callable<String>() {
			@Override
			public String call() throws Exception {
				return executeCommand("/home/johesbater/Projects/emp/empsql/exec.sh");
			}
			
			private String executeCommand(String command) {
				try {
					ByteArrayOutputStream stdout = new ByteArrayOutputStream();
			        PumpStreamHandler psh = new PumpStreamHandler(stdout);

			        CommandLine cl = CommandLine.parse(command);

			        DefaultExecutor exec = new DefaultExecutor();
			        exec.setStreamHandler(psh);
			        exec.execute(cl);
					return stdout.toString();					
				} catch (Exception e) {
					return "Failed to execute command";
				}
			}
		});
		
		for (String result : results)
			System.out.println(result);
	}

}
