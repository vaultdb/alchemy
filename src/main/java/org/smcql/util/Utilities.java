package org.smcql.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import org.apache.calcite.adapter.java.JavaTypeFactory;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rel.type.RelRecordType;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Table;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.math3.distribution.GeometricDistribution;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.schema.SecureSchemaLookup;
import org.smcql.executor.smc.OperatorExecution;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;
import org.smcql.type.SecureRelRecordType;


public class Utilities {
	

	
	public static String getSMCQLRoot() {
		String root = System.getProperty("smcql.root"); // for remote systems
	    if(root != null && root != "") {
	    		return root;
	    }
	       
	    // fall back to local path
	    URL location = Utilities.class.getProtectionDomain().getCodeSource().getLocation();
	    String path = location.getFile();
	       
	    // chop off trailing "/bin/src/"
	    if(path.endsWith("src/")) { // ant build
	        path = path.substring(0, path.length()-"src/".length());
	    }
	       
	    if(path.endsWith("bin/")) { // eclipse and ant build
	    		path = path.substring(0, path.length() - "/bin/".length());
	    }
	       
	    if(path.endsWith("target/classes/")) 
	    		path = path.substring(0, path.length() - "/target/classes/".length());

	    if(path.endsWith(".jar"))
	    	path = path.substring(0, path.length() - "target/smcql-open-source-0.5.jar/".length());
	    
	    return path;
	}	
	 
	 
    	public static String getCodeGenRoot() {
    		String mpcLib = "";
    		try {
    			mpcLib =  SystemConfiguration.getInstance().getProperty("mpc-lib");
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    		return getSMCQLRoot() + "/conf/smc/operators/" + mpcLib;
    	}
 
    	public static String getCodeGenTarget() {
    	
    		try {
    			
    			SystemConfiguration config = SystemConfiguration.getInstance();
    			String nodeType = config.getProperty("node-type"); // local or remote
    			String localTarget = (nodeType.equals("remote")) ? config.getProperty("remote-codegen-target")
    															 : config.getProperty("local-codegen-target");
    			
    				return getSMCQLRoot() + "/" + localTarget;
    			
			} catch (Exception e) {
				System.err.println("Missing local-codegen-target parameter (or remote-codegen-target), please add it to your config file.");
				e.printStackTrace();
			}
    		// default 
    		return getSMCQLRoot() + "/bin";
    	}
 
    	public static SecureRelRecordType getOutSchemaFromSql(String sql) throws Exception {
			SecureRelRoot relRoot = new SecureRelRoot("anonymous", sql);
			
			return relRoot.getPlanRoot().getSchema();
			
		}


		public static boolean isCTE(OperatorExecution src) {
			String packageName = src.packageName;
			String pkg = packageName.substring(packageName.lastIndexOf('.') + 1);
			pkg = pkg.replaceAll("\\d", "");
			return pkg.equals("CommonTableExpressionScan");
		}
		public static void mkdir(String path) throws Exception {
			
			String cmd = "mkdir -p "  + path;
			
			CommandOutput output = runCmd(cmd);
			
			if(output.exitCode != 0 && output.exitCode != 1) { // 1 = already exists
				throw new Exception("Failed to create path " + path + "!");
			}
			
			
		}
		
	public static void cleanDir(String path) throws Exception {
			
			String cmd = "rm -rf "  + path + "/*" ;
			CommandOutput output = runCmd(cmd);
			
			if(output.exitCode != 0) {
				throw new Exception("Failed to clear out " + path + "!");
			}
			
			
		}

		
	public static CommandOutput runCmd(String aCmd) throws IOException, InterruptedException {
		
		String[] cmd = StringUtils.split(aCmd, ' ');
		return runCmd(cmd, null);
	}
	
public static CommandOutput runCmd(String aCmd, String aWorkingDirectory) throws IOException, InterruptedException {
		
		String[] cmd = StringUtils.split(aCmd, ' ');
		return runCmd(cmd, aWorkingDirectory);
	}
	
	
	
	public static CommandOutput runCmd(String[] cmd, String workingDirectory) throws IOException, InterruptedException {

		
		if(workingDirectory == null || workingDirectory == "") {
			workingDirectory = Utilities.getSMCQLRoot();
		}
		
		File dir = new File(workingDirectory);
		
		Process p = java.lang.Runtime.getRuntime().exec(cmd, null, dir);

		BufferedReader stderr = new BufferedReader(new InputStreamReader(p.getErrorStream()));
		BufferedReader stdout = new BufferedReader(new InputStreamReader(p.getInputStream()));

		String err, out;
		
		CommandOutput cmdOutput = new CommandOutput();
		
		err = stderr.readLine();
		out = stdout.readLine();
		while(err != null || out != null) {
			if(err != null) {
				cmdOutput.output += err + "\n";
				err = stderr.readLine();
			}
			if(out != null) {
				cmdOutput.output += out + "\n";
				out = stdout.readLine();
			}
		}		
		
		p.waitFor();
		
		cmdOutput.exitCode = p.exitValue();
		return cmdOutput;
	}


	public static String getOperatorId(String packageName) {
		int idx = packageName.lastIndexOf('.');
		return packageName.substring(idx+1, packageName.length());
	
	}

	public static String getTime() {
        Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("MM/dd/yy HH:mm:ss");
       return sdf.format(cal.getTime());
	}

	public static float getElapsed(Date start, Date end) {
		return  (end.getTime() - start.getTime()) / 1000F; // ms --> secs
		
	}

	public static boolean dirsEqual(String lhs, String rhs) throws IOException, InterruptedException {
		String cmd = "diff -r " + lhs + " " + rhs;


		CommandOutput output =  runCmd(cmd);
		if(output.exitCode != 0) {
			System.out.println("diff: " + output.output);
		}
		return output.exitCode == 0;
	}

	
	public static boolean isUnion(OperatorExecution op) {
		if(op.packageName.endsWith(".merge"))
			return true;
		return false;
	}


	public static SecureRelDataTypeField lookUpAttribute(String table, String attr) throws Exception {
		SystemConfiguration conf = SystemConfiguration.getInstance();
		SchemaPlus tables = conf.getPdfSchema();
		Table lookupTable = tables.getTable(table);
		JavaTypeFactory typeFactory = conf.getCalciteConnection().getTypeFactory();
		
		RelRecordType rowType = (RelRecordType) lookupTable.getRowType(typeFactory);
		RelDataTypeField fieldType = rowType.getField(attr, false, false);
		
		SecureSchemaLookup lookup = SecureSchemaLookup.getInstance();
		SecurityPolicy policy = lookup.getPolicy(table, attr);
		
		SecureRelDataTypeField result = new SecureRelDataTypeField(fieldType, policy);
		result.setStoredAttribute(attr);
		result.setStoredTable(table);
		return result;
	}
	
	// partition key helps us deduce what tables can be joined unencrypted (locally) with oblivious padding
	public static boolean isLocalPartitionKey(SecureRelRecordType table, SecureRelDataTypeField attr) throws Exception {
		
		// only supporting a single partition key defined for now
		SystemConfiguration config = SystemConfiguration.getInstance();

		String srcTable = attr.getStoredTable();
		System.out.println("Analyzing " + attr + " src table: " + srcTable);

		
		SecureSchemaLookup schemaDef = SecureSchemaLookup.getInstance();
		List<String> primaryKey = schemaDef.getPrimaryKey(srcTable);
		
		
		if(attr.getName() == null)
			throw new Exception("Can't check status of anonymous attribute! Need to recurse to its inputs.");
		// Case 1: if it is the source relation's stored partition key
		if(schemaDef.getPartitionKey(srcTable).equals(attr.getName()))
		{
			return true;
		}
		

		// Case 2: the partition key is not a match and no primary keys
		if(primaryKey == null) {
			return false;
		}


		// Case 3: if there is a primary key, then any partition key will automatically divide this up
		// by primary key since the latter admits no duplicates
		if(primaryKey.contains(attr.getName()) &&  primaryKey.size() == 1 && schemaDef.getPartitionKey(srcTable) != null) {
			return true;
		}
			
		// TODO: remove hardcode, lineitem is a special case where linenumber will be partitioned along with orderkey
		// owing to its semantics of counting the items in an order
		if(config.getProperty("schema-name").equals("tpch") && attr.getName().equals("l_orderkey")) {
				return true;
		}
			
			
		 // else false
		return false;
	}

	

	public static int generateDiscreteLaplaceNoise(double epsilon, double delta, double sensitivity) {
		double prob = 1.0 - Math.exp(-1.0 * epsilon/sensitivity);
		double lpMean = Math.ceil(-1.0*sensitivity * Math.log( (Math.exp(epsilon/sensitivity)+1) * (1.0-Math.pow(1.0-delta, 1.0/sensitivity)) )/epsilon);
		
		GeometricDistribution geo = new GeometricDistribution(prob);
		int positiveSide = geo.sample() - 1;
		int negativeSide = geo.sample() - 1;
		
		return (int) (positiveSide - negativeSide + lpMean);
	}
	
	
	public static String getDistributedQuery(String query) throws Exception {
		SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(query);
		String tableName = outSchema.getAttributes().get(0).getStoredTable();
		return query.replaceFirst(tableName, "((SELECT * FROM remote_" + tableName +"_A) UNION ALL (SELECT * FROM remote_" + tableName + "_B)) remote_" + tableName);

	}


	public static void mkdir(String path, String workingDirectory) throws Exception {
		
		String cmd = "mkdir -p "  + path;
		
		CommandOutput output = runCmd(cmd, workingDirectory);
		
		if(output.exitCode != 0 && output.exitCode != 1) { // 1 = already exists
			throw new Exception("Failed to create path " + path + "!");
		}
		
		
	}

}
