package org.vaultdb.compiler.emp;

import org.vaultdb.BaseTest;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpProgram;
import org.vaultdb.util.EmpJniUtilities;


public class EmpBuilderTest extends BaseTest {

	protected void setUp() throws Exception {
		super.setUp();
	}
	



	// tests emp-jni link with minimal dependencies
	public void testEmpJniDemo() throws Exception {	
		String className = "org.vaultdb.compiler.emp.generated.EmpJniDemo";
		String expectedClass = "I am a org.vaultdb.compiler.emp.generated.EmpJniDemo!";
		
		testCase(className, expectedClass);	
	}
	
	
	// tests emp-jni link with jdbc for data inputs
	public void testCount() throws Exception {
		String className = "org.vaultdb.compiler.emp.generated.Count";		
		String expectedClass = "I am a org.vaultdb.compiler.emp.generated.Count!";
		
		testCase(className, expectedClass);	
	}

	protected void testCase(String className, String expectedClass) throws Exception {		
		EmpJniUtilities.buildEmpProgram(className);

		EmpBuilder builder = new EmpBuilder(className);
		EmpProgram instance = builder.getClass(1, 54321);
		assert(instance != null);
		
		// verify that it loads and is runnable
		String observedClass = instance.helloWorld();
		assertEquals(observedClass, expectedClass);
	}
	
}
