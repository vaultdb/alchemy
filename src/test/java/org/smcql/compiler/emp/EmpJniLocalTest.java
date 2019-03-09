package org.smcql.compiler.emp;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


import org.smcql.util.EmpJniUtilities;
import junit.framework.TestCase;

public class EmpJniLocalTest  extends TestCase  {

	
	final String fullyQualifiedClassName = "org.smcql.compiler.emp.generated.EmpJniDemo";
    final int tupleWidth = 3; //characters in string
	 

    
	public void testEmpJniProcessCalls() throws Exception {
		EmpRunnable aliceRunnable = new EmpRunnable(fullyQualifiedClassName, 1, 54321, false);
		EmpRunnable bobRunnable = new EmpRunnable(fullyQualifiedClassName, 2, 54321, false);


	   EmpBuilder builder = new EmpBuilder();
	   builder.compile(fullyQualifiedClassName, true);

	   
		
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
