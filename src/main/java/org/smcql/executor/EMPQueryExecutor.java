package org.smcql.executor;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;

import org.apache.commons.lang3.StringUtils;
import org.gridkit.nanocloud.Cloud;
import org.gridkit.nanocloud.CloudFactory;
import org.gridkit.nanocloud.RemoteNode;
import org.gridkit.nanocloud.VX;
import org.gridkit.nanocloud.telecontrol.ssh.SshSpiConf;
import org.gridkit.vicluster.ViNode;
import org.gridkit.vicluster.ViProps;
import org.gridkit.vicluster.telecontrol.Classpath;
import org.gridkit.vicluster.telecontrol.ssh.RemoteNodeProps;
import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.smc.SecureBufferPool;
import org.smcql.util.Utilities;

import com.oblivm.backend.flexsc.Party;


public class EMPQueryExecutor implements Runnable {
	String empCode;
	String remotePath;
	List<WorkerConfiguration> workers;
	Cloud cloud;
	
	public EMPQueryExecutor(String empCode, List<String> parties) throws Exception {
		if (workers.size() > 2) 
			throw new Exception("SMCQL does not support execution for more than two parties!");
		
		this.empCode = empCode;		
		this.workers = new ArrayList<WorkerConfiguration>();
		
		for (String workerId : parties) 
			workers.add(ConnectionManager.getInstance().getWorker(workerId));
		
		remotePath = SystemConfiguration.getInstance().getProperty("remote-path");
		if(remotePath == null)
			remotePath = "/tmp/smcql";
		
		prepareWorkers();
	}
	
	private void prepareWorkers() throws Exception {
		WorkerConfiguration aliceWorker = workers.get(0);
		WorkerConfiguration bobWorker = workers.get(1);
		
		System.out.println("Executing query on workers: " + aliceWorker + "(" + aliceWorker.hostname 
				+ ") and " + bobWorker + "(" + bobWorker.hostname + ")");
		
		cloud = CloudFactory.createCloud();
		RemoteNode.at(cloud.node("**")).useSimpleRemoting();

		//initialize hosts
		initializeHost(aliceWorker);
		initializeHost(bobWorker);
		
		String bufferPoolPointers = SecureBufferPool.getInstance().getPointers();
		
		cloud.node("**").setProp("plan.pointers", bufferPoolPointers);
		cloud.node("**").setProp("smcql.setup.str", getSetupParameters());
		cloud.node("**").setProp("smcql.connections.str", getConnectionParameters());
		cloud.node("**").setProp("smcql.root", "");

		// configure Alice and Bob
		cloud.node(aliceWorker.workerId).setProp("party", "gen");
		cloud.node(aliceWorker.workerId).setProp("workerId", aliceWorker.workerId);
		   
		cloud.node(bobWorker.workerId).setProp("party", "eva");
		cloud.node(bobWorker.workerId).setProp("workerId", bobWorker.workerId);

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

		if(host.equalsIgnoreCase("localhost")) {
			cloudHost.x(VX.TYPE).setLocal();
			ViProps.at(cloudHost).setIsolateType(); // enable debugger
		}
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
	
	@Override
	public void run() {
		List<String> results = cloud.node("**").massExec(new Callable<String>() {
			@Override
			public String call() throws Exception {
				Party party = (System.getProperty("party").equals("gen")) ? Party.Alice : Party.Bob;
				String workerId = System.getProperty("workerId");
				
				//JSCH stuff here
				
				
				return "To be initialized!";
			}
		});
		
		System.out.println(results.get(0));
	}

}
