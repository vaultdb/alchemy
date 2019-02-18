package org.smcql.executor;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.PumpStreamHandler;
import org.apache.commons.lang3.StringUtils;
import org.gridkit.nanocloud.Cloud;
import org.gridkit.nanocloud.CloudFactory;
import org.gridkit.nanocloud.RemoteNode;
import org.gridkit.nanocloud.telecontrol.ssh.SshSpiConf;
import org.gridkit.vicluster.ViNode;
import org.gridkit.vicluster.telecontrol.Classpath;
import org.gridkit.vicluster.telecontrol.ssh.RemoteNodeProps;
import org.smcql.codegen.QueryCompiler;
import org.smcql.codegen.smc.compiler.emp.EmpCompiler;
import org.smcql.codegen.smc.compiler.emp.EmpProgram;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.smc.SecureBufferPool;
import org.smcql.executor.smc.SecureQueryTable;
import org.smcql.executor.smc.runnable.RunnableSegment;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

import com.oblivm.backend.flexsc.Party;
import com.oblivm.backend.gc.GCSignal;


public class EMPExecutor implements Runnable {
	String remotePath;
	Cloud cloud;
	QueryCompiler compiledPlan = null;
	List<boolean[]> results;
	
	public EMPExecutor(QueryCompiler qc, List<String> parties) throws Exception {
		if (parties.size() > 2) 
			throw new Exception("SMCQL does not support execution for more than two parties!");
		
		
		List<WorkerConfiguration> workers = new ArrayList<WorkerConfiguration>();
		
		for (String workerId : parties) 
			workers.add(ConnectionManager.getInstance().getWorker(workerId));
		
		remotePath = SystemConfiguration.getInstance().getProperty("remote-path");
		if(remotePath == null)
			remotePath = "/tmp/smcql";
		
		prepareWorkers(workers);
		compiledPlan = qc;
		
	}
	
	private void prepareWorkers(List<WorkerConfiguration> workers) throws Exception {
		cloud = CloudFactory.createCloud();
		RemoteNode.at(cloud.node("**")).useSimpleRemoting();
		
		for (WorkerConfiguration w : workers) {
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
		
		String queryName = compiledPlan.getPlan().getName();
		String buildCmd = worker.empBridgePath + "/build.sh " + worker.empBridgePath + "/src/" + queryName + ".cpp";

		
		String execCmd = worker.empBridgePath + "/bin/" +  queryName  + " " + worker.dbId +  " "	 + worker.empPort;
		ViNode cloudHost = cloud.node(workerId);		
		RemoteNodeProps.at(cloudHost).setRemoteHost(host);
		cloudHost.setProp(SshSpiConf.SPI_JAR_CACHE, remotePath);
		cloud.node(worker.workerId).setProp("workerId", worker.workerId);
		cloud.node(worker.workerId).setProp("buildCmd", buildCmd);
		cloud.node(worker.workerId).setProp("execCmd", execCmd);
		
		
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
	
	@Override
	public void run() {
			
		 results = cloud.node("**").massExec(new Callable<boolean[]>() {
				@Override
				public boolean[] call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;
					
					EmpCompiler compiler = new EmpCompiler(compiledPlan.getPlan().getName());
					compiler.compile();
					
					EmpProgram runner = EmpCompiler.loadClass(compiledPlan.getPlan().getName(), party, 54321);
					runner.runProgram();
	
					boolean[] smcOutput = runner.getOutput(); 
				
					return smcOutput;
				}
			});
		 
	
	 }
	 
	
			
	
	public QueryTable getOutput() throws Exception {
		if (results == null || results.isEmpty())
			return null;
		
		boolean[] alice = results.get(0);
		boolean[] bob = results.get(1);
		boolean[] decrypted = null;
		
		// TODO: create two boolean[] arrays, one for alice, and one for bob
		// write the output of xoring them to decrypted variable
		
		SecureRelRecordType rootSchema = compiledPlan.getPlan().getPlanRoot().getSchema();
		return new QueryTable(decrypted,  rootSchema);
	}
	

}
