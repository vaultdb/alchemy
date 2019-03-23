package org.smcql.compiler.emp;

import java.io.File;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.Arrays;
import java.util.Properties;

import org.bytedeco.javacpp.ClassProperties;
import org.bytedeco.javacpp.LoadEnabled;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.tools.BuildEnabled;
import org.bytedeco.javacpp.tools.Builder;
import org.bytedeco.javacpp.tools.Logger;
import org.smcql.config.SystemConfiguration;
import org.smcql.util.CommandOutput;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.Utilities;


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
    

    @SuppressWarnings("rawtypes")
    public void compile() throws Exception {
    	
    	// node type is local or remote
    	// local nodes run within maven framework and work out of src/main/java
    	// remote simply works from "."
    	String nodeType = SystemConfiguration.getInstance().getProperty("node-type");

    	    
        Properties properties = getProperties();
        
        
        String className = fullyQualifiedClassName.substring(fullyQualifiedClassName.lastIndexOf('.')+1);

        if(nodeType.equalsIgnoreCase("local")) {
        	// copy over our header files to build target for local builds
        	String srcHeader = "src/main/java/org/smcql/compiler/emp/generated/" + className + ".h";
        	String dstHeader = "target/classes/org/smcql/compiler/emp/generated/" + className + ".h";

        	// in localhost setting
        	String cmd = "cp " + srcHeader + " " + dstHeader;

        	String cwd = System.getProperty("user.dir");
        	CommandOutput out = Utilities.runCmd(cmd, cwd);
        	if(out.exitCode != 0) {
        		throw new Exception("File copy failed!");
        	}
        }
        
        Builder builder = new Builder().properties(properties).classesOrPackages(fullyQualifiedClassName); //.copyLibs(true);

        
        File[] outputFiles = builder.build();
        smcqlLogger.info("Builder files: " + Arrays.toString(outputFiles));
        if(outputFiles == null || outputFiles.length == 0) {
        	System.out.println("Code compilation failed!");
        	System.exit(-1);
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

        
        String localLinkPath = System.getProperty("user.dir") + "/org/smcql/compiler/emp/generated/" + Loader.getPlatform();
        
        linkPath += ":" + localLinkPath;
        properties.setProperty("platform.linkpath", linkPath);

        return properties;
        
    }
}
