package org.smcql.compiler.emp.generated;


import java.nio.ByteBuffer;

import org.bytedeco.javacpp.BoolPointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.ByPtr;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;

import java.util.Properties;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.IOException;




import org.smcql.compiler.emp.EmpProgram;
import org.smcql.compiler.emp.EmpParty;


@Platform(include={"Count.h"}, 
			compiler = "cpp11")





@Namespace("Count")
public class Count  extends EmpProgram  {


	
	public Count(EmpParty aParty) {
		super(aParty);
	}

	public static class CountClass extends Pointer {
	
        static {
           // System.setProperty("java.class.path", "../jars/javacpp.jar:/Users/jennie/projects/alchemy/code/bin");
        
		Properties props = System.getProperties();
		props.remove("empCode");
		props.remove("jniCode");
		props.remove("smcql.setup.str");
		props.remove("smcql.connections.str");
		
            System.out.println("Loader environment: " + props); 
         
			Loader.load(); 
			/*         Loader.
        FileInputStream fis = null;
        Properties properties = new Properties();
         try {
          
          String propertyFilename = "/Users/jennie/projects/alchemy/code/src/main/resources/org/bytedeco/javacpp/properties/macosx-x86_64-emp.properties";
          File propertyFile = new File(propertyFilename);
          fis = new FileInputStream(propertyFile);
         }
         catch (IOException e) {
	         System.out.println("Could not load property file,  /Users/jennie/projects/alchemy/code/src/main/resources/org/bytedeco/javacpp/properties/macosx-x86_64-emp.properties, exiting...");
	         System.exit(-1);
         }
	      try {
             properties.load(new InputStreamReader(fis));
           } catch (Exception e) {
              properties.load(fis);
          }
        fis.close();
        Loader.load(CountClass.class, properties, false); 
			 */
	       } 
       
        public CountClass() { 	
        	allocate(); 
        	}
        private native void allocate();

        public native void run(int party, int port); 
        public native @ByPtr BoolPointer getOutput();
        
        
	}
	
	   boolean[] queryOutput = null;
	   
	   @Override
	   public String helloWorld() {
    		return "I am a Count!";
    	}
	   
	   	
        @Override
        public  boolean[] runProgram() {
        	System.out.println("Running Count");
        	CountClass theQuery = new CountClass();
        	
	        theQuery.run(party.asInt(), party.getPort());

		    System.out.println("Done running emp!");	        
	        BoolPointer outputPtr = theQuery.getOutput();
	        ByteBuffer buf = outputPtr.asByteBuffer();
	        
	        // copy bytes out of buffer
	        byte[] bytes = new byte[buf.capacity()];    
	        buf.get(bytes, 0, bytes.length);
	        
	        theQuery.close();
	        
	       return byteArray2BitArray(bytes);
	        
        }
        
	// for testing
	public static void main(String[] args) {
        // Pointer objects allocated in Java get deallocated once they become unreachable,
        // but C++ destructors can still be called in a timely fashion with Pointer.deallocate()
           int party = Integer.parseInt(args[0]);
	       int port = Integer.parseInt(args[1]);
		
		EmpParty theParty =  new EmpParty(party, port);
	Count qc = new Count(theParty);
	qc.runProgram();
	
	        
    }        
	
    	
}