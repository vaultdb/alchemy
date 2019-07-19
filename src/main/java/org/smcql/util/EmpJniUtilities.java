package org.smcql.util;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.bytedeco.javacpp.Loader;
import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpRunnable;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.smc.BasicSecureQueryTable;
import org.smcql.type.SecureRelRecordType;

public class EmpJniUtilities {
	
    public static String getPropertyFile()  {
	String propertyFile = Utilities.getSMCQLRoot() + "/src/main/resources/org/bytedeco/javacpp/properties/";
	propertyFile += Loader.getPlatform();
	propertyFile += "-emp.properties";
	 
	return propertyFile;
    }

	// output handling
	public static List<String> revealStringOutput(BitSet alice, BitSet bob, int tupleWidth) {
		assert(alice.size() == bob.size());
		
		BitSet decrypted = (BitSet) alice.clone();
		decrypted.xor(bob);
		
		int tupleBits = tupleWidth*8; // 8 bits / char
		int tupleCount = decrypted.size() / tupleBits;
		
		
		List<String> output = new ArrayList<String>();
		
		System.out.println("Decrypting " + tupleCount + " tuples.");
		
		int readIdx = 0;
		for(int i = 0; i < tupleCount; ++i) {
			BitSet bits = decrypted.get(readIdx*8, (readIdx+tupleWidth)*8);
			String tuple= deserializeString(bits);
			output.add(tuple);
			readIdx += tupleWidth;
		}
		
		return output;		
	}
	
	public static String deserializeString(BitSet src) {
		assert(src.size() % 8 == 0);
		int chars = src.size() / 8;
		String value = new String();
		
		
		for(int i = 0; i < chars; ++i)
		{
			int n = 0;
			 for(int j = 0; j < 8; ++j) {
			    	boolean b = src.get(i*8 + j);
			        n = (n << 1) | (b ? 1 : 0);
			    }
			 
			value += (char) n;
			
		}
		
		return value;
		
		
	}

	
	



	// class name includes package info
	// e.g., org.smcql.compiler.emp.generated.Count
	public static QueryTable runEmpLocal(String className, SecureRelRecordType outSchema) throws Exception {
		int empPort = getEmpPort();
		String fullyQualifiedClassName = getFullyQualifiedClassName(className);
		
		EmpRunnable aliceRunnable = new EmpRunnable(fullyQualifiedClassName, 1, empPort);
		EmpRunnable bobRunnable = new EmpRunnable(fullyQualifiedClassName, 2, empPort);

	   EmpBuilder builder = new EmpBuilder(fullyQualifiedClassName);
	   builder.compile();

	   
		
		Thread alice = new Thread(aliceRunnable);
		alice.start();
		
		Thread bob = new Thread(bobRunnable);
		bob.start();
		
		alice.join();
		bob.join();
		
		// output consists of dummyTags followed by padded tuple payload
		BitSet aliceOutput = aliceRunnable.getOutput();
		BitSet bobOutput = bobRunnable.getOutput();
		
		BasicSecureQueryTable aliceTable = new BasicSecureQueryTable(aliceOutput, outSchema);
		BasicSecureQueryTable bobTable = new BasicSecureQueryTable(bobOutput, outSchema);

		

		System.out.println("EMP output length" + aliceOutput.size());

		// decrypt dummies
		// selectively decrypt

		
		return aliceTable.declassify(bobTable);

		// old logic

		//boolean[] decrypted = decrypt(aliceOutput, bobOutput);
		//return new QueryTable(decrypted, outSchema);
		
	}
	 
	
	// for debugging this does a deep delete on previous builds 
	public static void cleanEmpCode(String className) throws Exception {
		String delGeneratedFiles = "rm " + Utilities.getCodeGenTarget() + "/" + className + "* ";
		String platform = Loader.getPlatform();
		String delGeneratedTargets = "rm " + Utilities.getSMCQLRoot() + "/target/classes/org/smcql/compiler/emp/generated/" + className + "*";
		String delOsCode = "rm -rf " + Utilities.getCodeGenTarget()+ "/" + platform + "/*";
		// nuke the javacpp cache
		String delCache = "rm -rf " + System.getProperty("user.home") + "/.javacpp/cache";
		
		
		Utilities.runCmd(delGeneratedFiles);
		Utilities.runCmd(delGeneratedTargets);
		Utilities.runCmd(delOsCode);		
		Utilities.runCmd(delCache);
		
		
		
	}

	public static void createJniWrapper(String className, String dstFile, Map<String, String> inputs) throws Exception {
		// if it is a fully qualified class name, strip the prefix
		if(className.contains(".")) {
			className = className.substring(className.lastIndexOf('.'+1));
		}

		Map<String, String> variables = new HashMap<String, String>();

		variables.put("queryName", className);
		variables.put("queryClass", className + "Class");

		if(inputs == null) {
			throw new Exception("Cannot run a query without input data!");
		}
		
		Iterator inputItr = inputs.entrySet().iterator();
		String inputSetup = new String();
		
		// generate sql input statements
		while(inputItr.hasNext()) {
			 Map.Entry pair = (Map.Entry)inputItr.next();
			 String sql = (String) pair.getValue();
			 sql = sql.replace('\n', ' ');
			 String putStatement = "inputs.put(\"" + pair.getKey() + "Union\", \"" + sql + "\");\n";
			 inputSetup += putStatement;
			 
		}
		
		variables.put("sqlSetup", inputSetup);
		
		String jniCode = CodeGenUtils.generateFromTemplate("/util/jni-wrapper.txt", variables);
		FileUtils.writeFile(dstFile, jniCode);
		
		Logger logger = SystemConfiguration.getInstance().getLogger();
	    logger.info("Writing jni wrapper to " + dstFile);
	}

	public static int getEmpPort() throws Exception {
		int port;
		// try local source
		String empPort = SystemConfiguration.getInstance().getProperty("emp-port");
		if(empPort != null && empPort != "") {
			port = Integer.parseInt(empPort); // TODO: check if it is numeric
		}
		else {
			// handle remote case
			port = Integer.parseInt(System.getProperty("emp.port"));
		}
		return port;
	}


	
	public static BitSet decrypt(BitSet alice, BitSet bob) {
		assert(alice.size() == bob.size());
		
		BitSet decrypted = (BitSet) alice.clone();
		decrypted.xor(bob);
		return decrypted;
		
	}

	public static String getFullyQualifiedClassName(String className) throws Exception {
		String classPrefix = SystemConfiguration.getInstance().getProperty("generated-class-prefix");
    	if(!className.startsWith(classPrefix)) {
    		return classPrefix + "." + className;
    	}
    	else {
    		return className;
    	}
    
	}
}
