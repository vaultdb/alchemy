package org.smcql.compiler.emp.generated;



import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.StdString;
import java.util.Map;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Iterator;

import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;
import org.smcql.executor.config.ConnectionManager;







import org.smcql.compiler.emp.EmpProgram;


@Platform(include={"Count.h"}, 
			compiler = "cpp11")





@Namespace("Count")
public class Count  extends EmpProgram  {

	Map<String, String> inputs = new HashMap<String, String>();
	boolean[] queryOutput = null;
	
	public Count(int party, int port) {
		super(party, port);
		inputs.put("Distinct2Merge", "SELECT DISTINCT icd9 FROM diagnoses ORDER BY icd9");
	}
	

	public static class CountClass extends Pointer {
	
        static {         
			Loader.load(); 
	       } 
       
        public CountClass() { 	
        	allocate(); 
        	}
        private native void allocate();
        public native void addInput(@StdString String opName,  @StdString String bitString);
        public native void run(int party, int port); 
        public native void setGeneratorHost(@StdString String host);
        public native @StdString String getOutput();
        
        
	}
	
	
	   
	   
		   	
        @Override
        public  void runProgram() throws Exception {
        	CountClass theQuery = new CountClass();

	        Iterator inputItr = inputs.entrySet().iterator();
	        while(inputItr.hasNext()) {
	        	Map.Entry entry = (Map.Entry) inputItr.next();
	        	String functionName = (String) entry.getKey();
	        	String sql = (String) entry.getValue();

	        	String table = getObliviousInput(functionName, sql);

                // add the input strings using the addInput function - which is available in main.txtOkay
	        	theQuery.addInput(functionName, table);
	        }
	        
        	if(generatorHost != null) {
        		theQuery.setGeneratorHost(generatorHost);
        	}
        	
        	
        	theQuery.run(party, port);
        	outputString = theQuery.getOutput();
	        theQuery.close();

	       	outputBits = super.stringToBitSet(outputString);
	        
        }
        



	// for testing on localhost only
	public static void main(String[] args) {
           
		int party = Integer.parseInt(args[0]);
		int port = Integer.parseInt(args[1]);
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		
  	    System.setProperty("smcql.setup", setupFile);
  	    String workerId = (party == 1) ? "alice" : "bob";
  	    
  	    
  	    
  	    System.setProperty("workerId", workerId);
  	    
		Count qc = new Count(party, port);
		BitSet bits = null;
		char b;
		
		try {
			SystemConfiguration.getInstance(); // initialize config
			qc.runProgram();
		} catch(Exception e) {
			System.err.println("Program execution failed!");
			e.printStackTrace();
			System.exit(-1);
		}
	     
		System.err.print(qc.getOutputString());
    
	        
    }        
	
    	
}
