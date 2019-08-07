package org.smcql.compiler.emp.generated;



import java.util.BitSet;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.StdString;





import org.smcql.compiler.emp.EmpProgram;
import org.vaultdb.util.EmpJniUtilities;


@Platform(include={"EmpJniDemo.h"}, 
			compiler = "cpp11")





@Namespace("EmpJniDemo")
public class EmpJniDemo  extends EmpProgram  {

	
	public EmpJniDemo(int party, int port) {
		super(party, port);
	}
	

	public static class EmpJniDemoClass extends Pointer {
	
        static {         
			Loader.load(); 
	       } 
       
        public EmpJniDemoClass() { 	
        	allocate(); 
        	}
        private native void allocate();
        public native void addInput(String opName, String bitString);
        public native void run(int party, int port); 
        public native void setGeneratorHost(@StdString String host);
        public native @StdString String getOutput();
        
        
	}
	
	
	   
	   	
        @Override
        public  void runProgram() {
        	EmpJniDemoClass theQuery = new EmpJniDemoClass();

        	if(generatorHost != null) {
        		theQuery.setGeneratorHost(generatorHost);
        	}
        	theQuery.run(party, port);
        	outputString = theQuery.getOutput();
	        theQuery.close();

	        
	        outputBits = EmpJniUtilities.stringToBitSet(outputString);
	        
	       
	       
	   
	       
        }
        

        
	// for testing
	public static void main(String[] args) {
           
		int party = Integer.parseInt(args[0]);
		int port = Integer.parseInt(args[1]);
		
		EmpJniDemo qc = new EmpJniDemo(party, port);
		qc.runProgram();
		System.err.print(qc.getOutputString());
	    
	        
    }        
	
    	
}