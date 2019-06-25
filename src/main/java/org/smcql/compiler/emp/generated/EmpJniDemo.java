package org.smcql.compiler.emp.generated;



import java.util.BitSet;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.StdString;





import org.smcql.compiler.emp.EmpProgram;


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
	
	   boolean[] queryOutput = null;
	
	   
	   	
        @Override
        public  BitSet runProgram() {
        	EmpJniDemoClass theQuery = new EmpJniDemoClass();

        	if(generatorHost != null) {
        		theQuery.setGeneratorHost(generatorHost);
        	}
        	theQuery.run(party, port);
        	String output = theQuery.getOutput();
	        theQuery.close();

	        BitSet outBits = new BitSet(output.length());
	        
	       for(int i = 0; i < output.length(); ++i) {
	    	   outBits.set(i,  output.charAt(i) == '1' ? true : false);
	       }
	       return outBits;
	       
        }
        

	// for testing
	public static void main(String[] args) {
           
		int party = Integer.parseInt(args[0]);
		int port = Integer.parseInt(args[1]);
		
		EmpJniDemo qc = new EmpJniDemo(party, port);
		BitSet bits = qc.runProgram();
		char b;
	       
			// write bitstring to stderr
			for(int i = 0; i < bits.size(); ++i) {
				b = (bits.get(i)) ? '1' : '0';
				System.err.print(b);
			}

	        
    }        
	
    	
}