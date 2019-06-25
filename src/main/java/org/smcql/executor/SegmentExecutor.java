package org.smcql.executor;


import java.io.File;
import java.sql.Connection;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.Callable;
import java.util.logging.Level;
import java.util.logging.Logger;

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
import org.smcql.codegen.QueryCompiler;
import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpProgram;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.type.SecureRelRecordType;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.smc.ExecutionSegment;
import org.smcql.executor.smc.OperatorExecution;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;


public class SegmentExecutor {

	Cloud cloud = null;
	WorkerConfiguration aliceWorker, bobWorker;
	private static SegmentExecutor instance = null;
	String remotePath;
	Logger logger;
	QueryCompiler compiledPlan = null;
	// currently only supports one execution segment
	// generalize for multiple exec modes (e.g., sliced --> oblivious) later
	SecureRelRecordType outSchema; 
	
	public static SegmentExecutor getInstance() throws Exception {
		if(instance == null) {
			ConnectionManager cm = ConnectionManager.getInstance();
			List<WorkerConfiguration> workers = cm.getWorkerConfigurations();
			
			if(workers.size() >= 2) {
				String aWorkerId = workers.get(0).workerId;
				String bWorkerId = workers.get(1).workerId;
				instance = new SegmentExecutor(aWorkerId, bWorkerId);
			}
			

		}
		return instance;
	}
	
	
	// setup for remote execution
	// TODO: move this into a separate method after a check to see if we need remote execution
	protected SegmentExecutor(String aWorker, String bWorker) throws Exception {

        aliceWorker = ConnectionManager.getInstance().getWorker(aWorker);
        bobWorker = ConnectionManager.getInstance().getWorker(bWorker);
        logger = SystemConfiguration.getInstance().getLogger();

        remotePath = SystemConfiguration.getInstance().getProperty("remote-path");
        if(remotePath == null)
                remotePath = "/tmp/smcql";


        String msg = "Initializing segment executor for " + aWorker + ", " + bWorker + " on " + aliceWorker.hostname + "," + bobWorker.hostname;
        logger.info(msg);

        cloud = CloudFactory.createCloud();
        RemoteNode.at(cloud.node("**")).useSimpleRemoting();

        initializeHost(bobWorker);
        initializeHost(aliceWorker);

        cloud.node("**").setProp("smcql.setup.str", SystemConfiguration.getInstance().getSetupParameters());
        cloud.node("**").setProp("smcql.connections.str", getConnectionParameters());
        cloud.node("**").setProp("emp.port", String.valueOf(EmpJniUtilities.getEmpPort()));

        // configure Alice and Bob                                                                                                                                           
        cloud.node(aWorker).setProp("party", "gen");
        cloud.node(aWorker).setProp("workerId", aliceWorker.workerId);

        cloud.node(bWorker).setProp("party", "eva");
        cloud.node(bWorker).setProp("workerId", bobWorker.workerId);

        cloud.node("**").touch();


		logger.log(Level.INFO, "Completed initialization.");
	}
	
	public void setQueryCompiler(QueryCompiler qc) {
		compiledPlan = qc;
		outSchema = qc.getOutSchema();
	}
	
	public Cloud getCloud() {
		return cloud;
	}
	
	/*public List<SecureQueryTable> runSecureSegment(ExecutionSegment segment) throws Exception {
		return runSecure(segment);
	}*/
	

    private void initializeHost(WorkerConfiguration worker) throws Exception {
            String host = worker.hostname;
            String workerId = worker.workerId;

            ViNode cloudHost = cloud.node(workerId);


            if(isLocal(host)) {
                    cloudHost.x(VX.TYPE).setLocal();
                    ViProps.at(cloudHost).setIsolateType(); // enable debugger  
                    cloudHost.setProp("smcql.root", Utilities.getSMCQLRoot());
                    cloudHost.setProp("remote.node", "false");
            }
            else { // remote case
            	cloudHost.x(VX.RUNTIME).setProp("user.dir", remotePath + "/" + workerId);
                RemoteNodeProps.at(cloudHost).setRemoteHost(host);
                cloudHost.setProp(SshSpiConf.SPI_JAR_CACHE, remotePath + "/" + workerId);
                cloudHost.setProp("remote.node", "true");

            }

            cloudHost.x(VX.PROCESS).addJvmArg("-Xms4096m").addJvmArg("-Xmx4g");                                                                                        
            
     
    }
	private String getConnectionParameters() throws Exception {
		String srcFile = SystemConfiguration.getInstance().getProperty("data-providers");
		List<String> params = FileUtils.readFile(srcFile);
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

	 
	 public QueryTable runPlaintext(String workerId, String sql, SecureRelRecordType os) throws Exception {

			ViNode node = cloud.node(workerId);
			final SecureRelRecordType outSchema = (os == null) ?  Utilities.getOutSchemaFromSql(sql) : os;
			
			QueryTable output = node.exec(new Callable<QueryTable>() {
				@Override
				public QueryTable call() throws Exception {
					return SqlQueryExecutor.query(sql, outSchema, workerId);
				}
			});
			
			return output;
	 }

	 public List<QueryTable> runPlaintext(String sql, SecureRelRecordType outSchema) {
		 List<QueryTable> result = cloud.node("**").massExec(new Callable<QueryTable>() {
				@Override
				public QueryTable call() throws Exception {
					String workerId = System.getProperty("workerId");					
					return SqlQueryExecutor.query(sql, outSchema, workerId);
				}
			});
		
		 return result;
	 }

	 public void runSetupPlaintext(String workerId, String sql) {

			ViNode node = cloud.node(workerId);

			node.exec(new Callable<Void>() {
				@Override
				public Void call()  {
					String workerId = System.getProperty("workerId");
					try {
						SqlQueryExecutor.queryNoOutput(sql, workerId);
					} catch (Exception e) {
						e.printStackTrace();
					}
					return null;
				}
			});
			
	 }
	 
	 	 
	 public void runSetupPlaintext(String sql) {
		cloud.node("**").massExec(new Callable<Void>() {
				@Override
				public Void call()  {
					String workerId = System.getProperty("workerId");
					try {
						SqlQueryExecutor.queryNoOutput(sql, workerId);
					} catch (Exception e) {
						e.printStackTrace();
					}
					return null;
				}
		 });
	 }
	 
	/* public List<SecureQueryTable> runSecure(ExecutionSegment segment) {
		 String sql = segment.sliceComplementSQL;
		 
		 // initialize code
			
		 List<SecureQueryTable> result = cloud.node("**").massExec(new Callable<SecureQueryTable>() {
				@Override
				public SecureQueryTable call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;
					String workerId = System.getProperty("workerId");
					
					
					segment.party = party;
					segment.workerId = workerId;
					segment.sliceComplementSQL = sql;
					RunnableSegment<GCSignal> runner = new RunnableSegment<GCSignal>(segment);
					
				
					Thread execThread = runner.runIt();
					execThread.join();
	
					SecureQueryTable smcOutput = runner.getOutput(); 
				
					return smcOutput;
				}
			});
		 
		 return result;
	 }
	 */
	 
	 
	 
	 
	 public QueryTable runPlain(PlaintextStep step) throws Exception {
		 OperatorExecution op = step.getExec();
		 Connection c = SystemConfiguration.getInstance().getHonestBrokerConfig().getDbConnection();
		 String tableName = op.outSchema.getAttributes().get(0).getStoredTable();
		 String srcSql = op.getSourceSQL().replaceFirst(tableName, "((SELECT * FROM remote_" + tableName +"_A) UNION ALL (SELECT * FROM remote_" + tableName + "_B)) remote_" + tableName);
		 QueryTable tupleData = SqlQueryExecutor.query(op.outSchema, srcSql, c);
		 return tupleData;
	 }

	public QueryTable runSecure(ExecutionSegment segment, String queryId) throws Exception {
		if(isLocal(aliceWorker.hostname) && isLocal(bobWorker.hostname)) {
			return runSecureLocal(segment, queryId);
			
		}
		return runSecureRemote(segment, queryId);
	}

	
	private QueryTable runSecureLocal(ExecutionSegment segment, String queryId) throws Exception {
		return EmpJniUtilities.runEmpLocal(queryId, outSchema);
	}
	

	 // emp runner, remote instance
	// designed for both nodes being remote or (1 remote, 1 local)
	 private QueryTable runSecureRemote(ExecutionSegment segment, String className) throws Exception {
			
	
			
		 List<BitSet> result = cloud.node("**").massExec(new Callable<BitSet>() {
				@Override
				public BitSet call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;
					int port = EmpJniUtilities.getEmpPort();
	
					// initialized from smcql.setup.str node / system property
					SystemConfiguration config = SystemConfiguration.getInstance();
					
					config.setProperty("node-type", "remote");

					//System.setProperty("org.bytedeco.javacpp.logger.debug", "true");
					
					
					
					EmpBuilder builder = new EmpBuilder(className);
					builder.compile();
					
					EmpProgram program = builder.getClass(party, port);
					
					
			 
					
					BitSet smcOutput = program.runProgram();
					System.out.println("Completed query run on party " + party);

					
				
					return smcOutput;
				}
			});
		 
		
		BitSet decrypted = EmpJniUtilities.decrypt(result.get(0), result.get(1));
		return new QueryTable(decrypted, segment.outSchema, true);
	 }

	 
	private boolean isLocal(String host) {
		if(host.equalsIgnoreCase("localhost") || host.equalsIgnoreCase("127.0.0.1"))
			return true;
		return false;
	}
	 
	
	

}
