package org.smcql.executor.emp.stub;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpRunnable;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.Utilities;

import junit.framework.TestCase;

public class EmpJniLocalTest  extends TestCase  {

	
	final String fullyQualifiedClassName = "org.smcql.compiler.emp.generated.EmpJniDemo";
    final int tupleWidth = 3; //characters in string
	 

    
	public void testEmpJni() throws Exception {
		int empPort = EmpJniUtilities.getEmpPort();
		
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
		
		boolean[] aliceOutput = aliceRunnable.getOutput();
		boolean[] bobOutput = bobRunnable.getOutput();
		
		List<String> output = EmpJniUtilities.revealStringOutput(aliceOutput, bobOutput, tupleWidth);
		System.out.println("Query output: " + output);

		List<String> expectedOutput = new ArrayList<>(Arrays.asList("008", "414", "008", "414"));
		assertEquals(expectedOutput, output);
		
	}
	
	
	

    
	 
	 

	 
	 


}
