package org.smcql.executor;


import java.io.File;
import java.sql.Connection;
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
import org.smcql.compiler.emp.EmpCompiler;
import org.smcql.compiler.emp.EmpParty;
import org.smcql.compiler.emp.EmpProgram;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.type.SecureRelRecordType;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.executor.smc.ExecutionSegment;
import org.smcql.executor.smc.OperatorExecution;
import org.smcql.executor.smc.SecureQueryTable;
import org.smcql.executor.smc.runnable.RunnableSegment;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.util.Utilities;

import com.oblivm.backend.gc.GCSignal;

public class SegmentExecutor {

	Cloud cloud = null;
	WorkerConfiguration aliceWorker, bobWorker;
	private static SegmentExecutor instance = null;
	String remotePath;
	Logger logger;
	QueryCompiler compiledPlan = null;
	
	
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

        cloud.node("**").setProp("smcql.setup.str", getSetupParameters());
        cloud.node("**").setProp("smcql.connections.str", getConnectionParameters());
        cloud.node("**").setProp("emp.port", String.valueOf(Utilities.getEmpPort()));

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
	}
	
	public Cloud getCloud() {
		return cloud;
	}
	
	public List<SecureQueryTable> runSecureSegment(ExecutionSegment segment) throws Exception {
		return runSecure(segment);
	}
	

    private void initializeHost(WorkerConfiguration worker) throws Exception {
            String host = worker.hostname;
            String workerId = worker.workerId;

            ViNode cloudHost = cloud.node(workerId);


            if(host.equalsIgnoreCase("localhost") || host.equalsIgnoreCase("127.0.0.1")) {
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

            cloudHost.x(VX.CLASSPATH).inheritClasspath(true);
            System.out.println("Parent classpath: " + System.getProperty("java.class.path"));
            cloudHost.x(VX.PROCESS).addJvmArg("-Xms4096m").addJvmArg("-Xmx10g");                                                                                        
            
     
    }
    private String getSetupParameters() throws Exception {
		
		
		Map<String, String> propertiesMap = SystemConfiguration.getInstance().getProperties();
		String properties = new String();
		
		for(Entry<String, String> param : propertiesMap.entrySet()) {
			properties += param.getKey() + "=" + param.getValue() + "\n";
		}
		
		return properties;
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
	 
	 public List<SecureQueryTable> runSecure(ExecutionSegment segment) {
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
	 
	 
	 // emp runner
	 public List<boolean[]> runSecure(ExecutionSegment segment, String className) throws Exception {
			
	
			
		 List<boolean[]> result = cloud.node("**").massExec(new Callable<boolean[]>() {
				@Override
				public boolean[] call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;

					String codegenWorkingDirectory = SegmentExecutor.getCodegenWorkingDirectory(party);
					System.setProperty("user.dir", codegenWorkingDirectory);
					//System.setProperty("org.bytedeco.javacpp.logger.debug", "true");
					
					
					System.out.println("Starting to launch emp on party " + party);
					
					
					int port = Utilities.getEmpPort();
					EmpParty partyObj = new EmpParty(party, port);
					EmpCompiler compiler = new EmpCompiler(className, partyObj);		
				    EmpProgram program = compiler.loadClass();
					System.out.println("Loaded emp!");
					
					
			 
					
					boolean[] smcOutput = program.runProgram();
					System.out.println("Completed query run on party " + party);

					
				
					return smcOutput;
				}
			});
		 
		 return result;
	 }
	 
	 /* private Integer compileCode(String worker) {
		 Integer  result = cloud.node(worker).exec(new Callable<Integer>() {
				@Override
				public Integer call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;
					int port = Utilities.getEmpPort();
					String className = System.getProperty("className");
					
					EmpParty theParty = new EmpParty(party, port);
					EmpCompiler compiler = new EmpCompiler(className, theParty);
					
					compiler.writeEmpCode(System.getProperty("empCode"));	
					compiler.writeJniWrapper(System.getProperty("jniCode"));
					compiler.setGenerateWrapper(false);
					return compiler.compile();		
				}

		 });
		 return result;
	 }
	 */
	 
	 protected static String getCodegenWorkingDirectory(int party) throws Exception {
		 SystemConfiguration config = SegmentExecutor.initializeSystemConfiguration(party);
		 String currentWorkingDirectory = System.getProperty("user.dir");
		 String javaCppWorkingDirectory = config.getProperty("javacpp-working-directory");
	     return currentWorkingDirectory + "/" + javaCppWorkingDirectory;
	}

	// both need to run unconditionally to make them sync up easily
	 private List<Integer> compileCode() {
		 List<Integer>  result = cloud.node("**").massExec(new Callable<Integer>() {
				@Override
				public Integer call() throws Exception {
					int party = (System.getProperty("party").equals("gen")) ? 1 : 2;
					int port = Utilities.getEmpPort();
					String className = System.getProperty("className");

					
					String codegenWorkingDirectory = SegmentExecutor.getCodegenWorkingDirectory(party);
					System.setProperty("user.dir", codegenWorkingDirectory);

					// if it is marked for compilation
					if(System.getProperty("jniCode") != null) {

						EmpParty theParty = new EmpParty(party, port);
						EmpCompiler compiler = new EmpCompiler(className, theParty);
						
						compiler.writeEmpCode(System.getProperty("empCode"));	
						compiler.writeJniWrapper(System.getProperty("jniCode"));
						compiler.setGenerateWrapper(false);
						return compiler.compile();		

						
					}
					return 0;
				}

		 });
		 return result;
	 }
	 
	 
	 public static  SystemConfiguration initializeSystemConfiguration(int party) throws Exception {
		// load setup
			String setupFile = "/tmp/smcql/setup" + party;
			String setupProperties = System.getProperty("smcql.setup.str");
			Utilities.writeFile(setupFile, setupProperties);
		    System.setProperty("smcql.setup", setupFile);
		    return SystemConfiguration.getInstance();
	 }
	 
	 public QueryTable runPlain(PlaintextStep step) throws Exception {
		 OperatorExecution op = step.getExec();
		 Connection c = SystemConfiguration.getInstance().getHonestBrokerConfig().getDbConnection();
		 String tableName = op.outSchema.getAttributes().get(0).getStoredTable();
		 String srcSql = op.getSourceSQL().replaceFirst(tableName, "((SELECT * FROM remote_" + tableName +"_A) UNION ALL (SELECT * FROM remote_" + tableName + "_B)) remote_" + tableName);
		 QueryTable tupleData = SqlQueryExecutor.query(op.outSchema, srcSql, c);
		 return tupleData;
	 }

	public void compileEmp(String className) throws Exception {
	 	String aWorker = ConnectionManager.getInstance().getAlice();
	 	String bWorker = ConnectionManager.getInstance().getBob();
	 	
	 	int empPort = Utilities.getEmpPort();
		EmpParty alice = new EmpParty(1, empPort);
		EmpParty bob = new EmpParty(2, empPort);
	 	
		EmpCompiler aliceCompiler = new EmpCompiler(className, new EmpParty()); // was alice
		EmpCompiler bobCompiler = new EmpCompiler(className, new EmpParty()); // was bob
		Boolean bothCompile = false;
		
		if(cloud.node(aWorker).getProp("remote.node").equals("true") ||
				cloud.node(bWorker).getProp("remote.node").equals("true"))
				bothCompile = true; // at least one remote node


		// configure Alice and Bob's jni wrapper for this segment
	 	cloud.node(aWorker).setProp("jniCode", aliceCompiler.getJniWrapperCode());
		cloud.node(aWorker).setProp("empCode", compiledPlan.generateEmpCode(alice));
		cloud.node(aWorker).setProp("className", className); //  + alice.asString());
		
		if(bothCompile) {	// eventually this will point to a single class
			cloud.node(bWorker).setProp("jniCode", bobCompiler.getJniWrapperCode());
			cloud.node(bWorker).setProp("empCode", compiledPlan.generateEmpCode(bob));
			cloud.node(bWorker).setProp("className", className + bob.asString());
			
		}
		compileCode(); // one or both compile code
	
		System.out.println("Finished compiling code!");
	}
	 
	
	 

}
