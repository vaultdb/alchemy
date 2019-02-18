package org.smcql.codegen.smc.compiler.emp;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.HashMap;
import java.util.Map;

import org.smcql.config.SystemConfiguration;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.CommandOutput;
import org.smcql.util.Utilities;

public class EmpCompiler {
	
	String empJavaWrapper;
	String javaCppJar;
	String workingDirectory;
	String propertyFile;
	String className;
	
	// e.g., Count
	public EmpCompiler(String className) throws Exception {
		this.className = className;
		SystemConfiguration config = SystemConfiguration.getInstance();
		
		empJavaWrapper = config.getProperty("codegen-target").replace("src/main/java/", "") +"/" + className + ".java";
		javaCppJar = config.getSmcqlRoot() + "/" + config.getProperty("javacpp-jar");
		workingDirectory = Utilities.getSMCQLRoot() + "/src/main/java";
		
		propertyFile = getPropertyFile();
		
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
		 
		 propertyFile += ".properties";
		 
		return propertyFile;
	}
	
	public int compile() throws IOException, InterruptedException {
		
		generateJniWrapper(); // create the JNI class for connecting to the emp code
		String command = "java -jar " + javaCppJar + " " + 
				empJavaWrapper + " -propertyfile " + propertyFile +
				" -nodelete";
		System.out.println("Compiler command: " + command);
		
		
		CommandOutput out = Utilities.runCmd(command, workingDirectory);
		
		System.out.println("Compiler output: " + out.output);
	
		return out.exitCode;
				

	}
	
	private void generateJniWrapper() throws IOException {
		Map<String, String> variables = new HashMap<String, String>();
		variables.put("queryName", className);
		variables.put("queryClass", className + "Class");

		String jniWrapperCode = CodeGenUtils.generateFromTemplate("/util/jni-wrapper.txt", variables);
		String jniFilename = Utilities.getCodeGenTarget() + "/" + className + ".java";
		Utilities.writeFile(jniFilename, jniWrapperCode);

	}
	
	// for the generated class
	public static String getGeneratedClassPrefix() throws Exception {
		String srcDir = SystemConfiguration.getInstance().getProperty("codegen-target");
		if(srcDir.endsWith("/")) {
			srcDir = srcDir.substring(0, srcDir.length()-1); // chop off trailing slash
		}
		srcDir = srcDir.replaceFirst("src/main/java/", "");  
		return srcDir.replace('/', '.');
	}
	
    @SuppressWarnings("unchecked")
	public static EmpProgram loadClass(String className, int party, int port) throws Exception {
            className = getGeneratedClassPrefix() + "." + className;
			File f = new File(Utilities.getCodeGenTarget());
			URL[] cp = {f.toURI().toURL()};
			@SuppressWarnings("resource")
			URLClassLoader urlcl = new URLClassLoader(cp);

			Class<?> cl = urlcl.loadClass(className);
			Constructor<?> ctor = cl.getConstructors()[0];
			return (EmpProgram) ctor.newInstance(party, port);
	
    }


}
