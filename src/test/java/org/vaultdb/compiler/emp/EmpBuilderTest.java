package org.vaultdb.compiler.emp;

import org.vaultdb.BaseTest;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpProgram;


public class EmpBuilderTest extends BaseTest {

	protected void setUp() throws Exception {
		super.setUp();
	}
	



	// tests emp-jni link with minimal dependencies
	public void testEmpJniDemo() throws Exception {
		String className = "org.vaultdb.compiler.emp.generated.EmpJniDemo";
		EmpBuilder builder = new EmpBuilder(className);
		builder.compile();
		EmpProgram instance = builder.getClass(1, 54321);
		assert(instance != null);
		// verify that it loads and is runnables
		assert(instance.helloWorld().equals("I am a org.vaultdb.compiler.emp.generated.EmpJniDemo!"));
		
	}
	
	
	// tests emp-jni link with jdbc for data inputs
	public void testCount() throws Exception {
		String className = "org.vaultdb.compiler.emp.generated.Count";
		EmpBuilder builder = new EmpBuilder(className);
		builder.compile();
		EmpProgram instance = builder.getClass(1, 54321);
		assert(instance != null);
		// verify that it loads and is runnable
		String observedClass = instance.helloWorld();
		String expectedClass = "I am a org.vaultdb.compiler.emp.generated.Count!";
		assertEquals(observedClass, expectedClass);
		
	}

}
