package org.smcql.compiler.emp;

import java.io.File;
import java.lang.reflect.Constructor;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

import org.bytedeco.javacpp.ClassProperties;
import org.bytedeco.javacpp.LoadEnabled;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.tools.BuildEnabled;
import org.bytedeco.javacpp.tools.Builder;
import org.bytedeco.javacpp.tools.Logger;
import org.smcql.util.Utilities;

// based on builder test in Javacpp
// https://github.com/bytedeco/javacpp/blob/master/src/test/java/org/bytedeco/javacpp/BuilderTest.java

public class EmpBuilder implements BuildEnabled, LoadEnabled {

  
    @Override 
    public void init(ClassProperties properties) {
    }

    @Override 
    public void init(Logger logger, Properties properties, String encoding) {
    }
    

    // takes as input fully qualified class name
    @SuppressWarnings("rawtypes")
    public void compile(String name, boolean isLocal) throws Exception {
        Class cls = Class.forName(name);
        
        // initial cleanup
        String extension = "-emp";
        URL u = Loader.findResource(cls, Loader.getPlatform() + extension);
        if (u != null) {
        	for (File f : new File(u.toURI()).listFiles()) {
        		f.delete();
                	}
            }
        

    
        Properties properties = getProperties();
        
        
        String className = name.substring(name.lastIndexOf('.')+1);
        if(isLocal) {
        	// copy over our header files to build target
        	String srcHeader = "src/main/java/org/smcql/compiler/emp/generated/" + className + ".h";
        	String dstHeader = "target/classes/org/smcql/compiler/emp/generated/" + className + ".h";
        	String cmd = "cp " + srcHeader + " " + dstHeader;

        	Utilities.runCmd(cmd, System.getProperty("user.dir"));
        }
        
        Builder builder = new Builder().properties(properties).classesOrPackages(cls.getName()); //.copyLibs(true);
 
        
        File[] outputFiles = builder.build();
        System.out.println("Builder files: " + Arrays.toString(outputFiles));
        
    }
    
    
    @SuppressWarnings("rawtypes")
    public EmpProgram getClass(String name, int party, int port) throws Exception {
        Class cls = Class.forName(name);
        Properties properties = getProperties();
        
        // loads and returns name of lib that contains this class
    	Object instance = Loader.load(cls, properties, true);
        if(instance == null || instance.equals("")) {
        	throw new Exception("Failed to load library for " + name);
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
