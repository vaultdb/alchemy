package org.vaultdb.compiler.emp;

import java.io.File;
import java.lang.reflect.Constructor;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.Properties;

import org.bytedeco.javacpp.ClassProperties;
import org.bytedeco.javacpp.LoadEnabled;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.tools.BuildEnabled;
import org.bytedeco.javacpp.tools.Builder;
import org.bytedeco.javacpp.tools.Logger;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.util.EmpJniUtilities;
import org.vaultdb.util.FileUtilities;
import org.vaultdb.util.Utilities;

import net.openhft.compiler.CachedCompiler;


public class EmpBuilder implements BuildEnabled, LoadEnabled {

	String fullyQualifiedClassName;
	java.util.logging.Logger smcqlLogger = null;
	
	public EmpBuilder(String className) throws Exception {
		fullyQualifiedClassName = EmpJniUtilities.getFullyQualifiedClassName(className);
    	smcqlLogger = SystemConfiguration.getInstance().getLogger();
	}
    @Override 
    public void init(ClassProperties properties) {
    }

    @Override 
    public void init(Logger logger, Properties properties, String encoding) {
    }
    
    @SuppressWarnings({"resource" })
	private void addToClasspath(String fullyQualifiedClassName, String path) throws Exception {
    	byte[] encoded = Files.readAllBytes(Paths.get(path));
    	String javaCode = new String(encoded, StandardCharsets.US_ASCII);	
    	String dstDir = Utilities.getSMCQLRoot() + "/target/classes";
    
    	CachedCompiler JCC = new CachedCompiler(null, new File(dstDir));
    	JCC.loadFromJava(fullyQualifiedClassName, javaCode);
    }

    public void compile() throws Exception {
    	
    	// node type is local or remote
    	// local nodes run within maven framework and work out of src/main/java
    	// remote simply works from "."
    	String nodeType = SystemConfiguration.getInstance().getProperty("node-type");

    	    
        Properties properties = getProperties();
        String className = fullyQualifiedClassName.substring(fullyQualifiedClassName.lastIndexOf('.')+1);
        
        
        File f = new File(Utilities.getSMCQLRoot() + "/target/classes/org/vaultdb/compiler/emp/generated/" + className + ".class");
        if(!f.exists()) { 
        	addToClasspath(fullyQualifiedClassName, "src/main/java/org/vaultdb/compiler/emp/generated/" + className + ".java");
        }

    	// in localhost setting
        if(nodeType.equalsIgnoreCase("local")) {
        	// copy over our header files to build target for local builds
        	String srcHeader = "src/main/java/org/vaultdb/compiler/emp/generated/" + className + ".h";
        	String dstHeader = "target/classes/org/vaultdb/compiler/emp/generated/" + className + ".h";

        	        	
        	String srcUtilities = "src/main/java/org/vaultdb/compiler/emp/generated/EmpUtilities.h";
        	String dstUtilities = "target/classes/org/vaultdb/compiler/emp/generated/EmpUtilities.h";

        	FileUtilities.copyFile(srcHeader, dstHeader);
        	FileUtilities.copyFile(srcUtilities, dstUtilities);

        }  
        
        smcqlLogger.info("Building class: " + fullyQualifiedClassName);
        Builder builder = new Builder().properties(properties).classesOrPackages(fullyQualifiedClassName).deleteJniFiles(true); //.copyLibs(true);
        File[] outputFiles = null;
        outputFiles = builder.build();
        try {
        	
        } catch(Exception e) {
          	throw new Exception("Code compilation failed!");
        }
        smcqlLogger.info("Builder files: " + Arrays.toString(outputFiles));
        if(outputFiles == null || outputFiles.length == 0) {
        	throw new Exception("No output files generated from jni compilation!");
        }
        
        
    }
    
    
    @SuppressWarnings("rawtypes")
    public EmpProgram getClass(int party, int port) throws Exception {
        Class cls = Class.forName(fullyQualifiedClassName);
        Properties properties = getProperties();
        
        // loads and returns name of lib that contains this class
    	Object instance = Loader.load(cls, properties, true);
        if(instance == null || instance.equals("")) {
        	throw new Exception("Failed to load library for " + fullyQualifiedClassName);
        }
        
        Constructor<?>[] constructors = cls.getConstructors();
    	Constructor<?> ctor = constructors[0];
		EmpProgram empInstance = (EmpProgram) ctor.newInstance(party, port);
		
		
        return empInstance;
    }
    
    public static String getPropertyFile()  {
    	String propertyFile = Utilities.getSMCQLRoot() + "/src/main/resources/org/bytedeco/javacpp/properties/";
    	propertyFile += Loader.getPlatform();
    	propertyFile += "-emp.properties";
	 
    	return propertyFile;
    }
    

    private Properties getProperties() {
    	String propertiesFile = Loader.getPlatform() + "-emp";
        Properties properties = Loader.loadProperties(propertiesFile, null);
        String linkPath = properties.getProperty("platform.linkpath");

        
        String localLinkPath = System.getProperty("user.dir") + "/target/classes/org/vaultdb/compiler/emp/generated/" + Loader.getPlatform();
        
        linkPath += ":" + localLinkPath;
        properties.setProperty("platform.linkpath", linkPath);

        return properties;
        
    }
}
