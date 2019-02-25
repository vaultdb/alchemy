package org.smcql.compiler.emp;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.smcql.compiler.emp.EmpProgram;
import org.smcql.compiler.DynamicCompiler;
import org.smcql.config.SystemConfiguration;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.CommandOutput;
import org.smcql.util.Utilities;
import java.util.logging.Level;
import java.util.logging.Logger;


public class EmpCompiler {
	
	String javaCppJar;
	String codegenTarget;
	String propertyFile;
	String className;
	boolean generateJni = true;
	Logger logger = null;
	EmpParty party;
	int port = 0;
	String javaCppWorkingDirectory;
	
	// e.g., Count
	public EmpCompiler(String className, EmpParty party) throws Exception {
		this.party = party;
		this.className = className;
		System.out.println("Working on  class " + this.className);
		
	    // JMR: undo this for Alice and Bob to have separate classes
		//this.className = className + party.asString();
		
		SystemConfiguration config = SystemConfiguration.getInstance();
		
		port = Integer.parseInt(config.getProperty("emp-port"));
		javaCppJar = Utilities.getSMCQLRoot() + "/" + config.getProperty("javacpp-jar");
		propertyFile = getPropertyFile();
		logger = config.getLogger();
		javaCppWorkingDirectory = Utilities.getSMCQLRoot() + "/" + config.getProperty("javacpp-working-directory");
		
		codegenTarget = Utilities.getCodeGenTarget();
	}
	
	private String getPropertyFile()  {
		String propertyFile = Utilities.getSMCQLRoot() + "/src/main/resources/org/bytedeco/javacpp/properties/";
		String osName  = System.getProperty("os.name", "").toLowerCase();
		String osArch  = System.getProperty("os.arch", "").toLowerCase();
		
		if (osName.startsWith("mac os x") || osName.startsWith("darwin")) {
            propertyFile += "macosx";
		}
		else if(osName.startsWith("linux")){
			propertyFile += "linux";
		}
		else {
			System.out.print("Unsupported system: " + osName + "!  Exiting...");
			System.exit(-1);
		}
		
		 if (osArch.equals("i386") || osArch.equals("i486") || osArch.equals("i586") || osArch.equals("i686")) {
	            propertyFile += "-x86";
	      }
		 else if (osArch.equals("amd64") || osArch.equals("x86-64") || osArch.equals("x64") || osArch.equals("x86_64")) {
	            propertyFile += "-x86_64";
	      }  
	      else  {
	    	  System.out.println("Unsupported architecture: " + osArch + "! Exiting...");
	      }
		 
		 propertyFile += "-emp.properties";
		 
		return propertyFile;
	}
	
	/*private String getJavaCppJar() {
		String classpath = System.getProperty("java.class.path");
		String[] paths =  classpath.split(":");
		
	}*/
	
	public void setGenerateWrapper(boolean val) {
		generateJni = val;
	}
	
	public int compile() throws Exception {
		String wrapperFile = getJniWrapperFilename();
		
		if(generateJni) { 
			generateJniWrapper(); // null for unknown or agnostic party
		}

		wrapperFile = wrapperFile.replace(javaCppWorkingDirectory + "/", "");
		logger.log(Level.FINE, "Wrapper file: " + wrapperFile);
		
		//String classPath = System.getProperty("java.class.path");
		
		String command = "java -jar " + javaCppJar + " " + 
				wrapperFile + " -propertyfile " + propertyFile +
				" -nodelete"; // -classpath " + classPath;
		
		logger.log(Level.INFO, "Compiler command: " + command);
		
		System.out.println("Compiler command: " + command);
		
		CommandOutput out = Utilities.runCmd(command, javaCppWorkingDirectory);
		

		if(out.exitCode != 0) {
			logger.log(Level.INFO,  "Query compilation failed.  Compiler output: " + out.output);
			System.exit(-1);
		}
	
		return out.exitCode;
				
	}
	
	private String getJniWrapperFilename() throws Exception {
		
		return codegenTarget + "/" + className  + ".java";
		
	}
	
	
	
	// generic filename
	public String getEmpFilename() throws Exception {

		return codegenTarget + "/" + className + ".h";
	}
	
	// only runs locally
	public  String getJniWrapperCode() throws IOException {
		Map<String, String> variables = new HashMap<String, String>();

		variables.put("queryName", className);
		variables.put("queryClass", className + "Class");
		variables.put("propertyFile", propertyFile);
		return CodeGenUtils.generateFromTemplate("/util/jni-wrapper.txt", variables);

	}
	
	public void writeJniWrapper(String jniWrapperCode) throws Exception {
		String jniFilename = getJniWrapperFilename();
		logger.log(Level.FINE, "Writing jni code to " + jniFilename);
		Utilities.writeFile(jniFilename, jniWrapperCode);
	}
	
	public void writeEmpCode(String empCode) throws Exception {
		String empFilename = getEmpFilename();
		logger.log(Level.INFO, "Writing emp code to " + empFilename);
		Utilities.writeFile(empFilename, empCode);
		

	}
	
	private String generateJniWrapper() throws Exception {
		String jniWrapperCode = getJniWrapperCode();
		String jniFilename = getJniWrapperFilename();	
		Utilities.writeFile(jniFilename, jniWrapperCode);
		return jniFilename;

	}
	
	
	// for the generated class
	public  String getGeneratedClassPrefix() throws Exception {
		String srcDir = new String(codegenTarget);
		srcDir = srcDir.replace(javaCppWorkingDirectory, ""); 
		if(srcDir.charAt(0) == '/') {
			srcDir = srcDir.substring(1);
		}
		return srcDir.replace('/', '.');
	}
	
    @SuppressWarnings("unchecked")
	public EmpProgram loadClass() throws Exception {
    		
    		String fullyQualifiedClassname = getGeneratedClassPrefix() + "." + className;
    		
            
            // do it in DynamicCompiler to garbage collect the classloader so that running locally won't create loader collisions in javacpp
            return DynamicCompiler.loadJniClass(fullyQualifiedClassname, party);		
			
    }


}
