package org.smcql.compiler.emp;

import org.smcql.BaseTest;
import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpProgram;
import org.smcql.util.Utilities;


public class EmpBuilderTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}
	



	// tests emp-jni link with minimal dependencies
	public void testEmpJniDemo() throws Exception {
		String className = "org.smcql.compiler.emp.generated.EmpJniDemo";
		EmpBuilder builder = new EmpBuilder(className);
		builder.compile();
		EmpProgram instance = builder.getClass(1, 54321);
		assert(instance != null);
		// verify that it loads and is runnables
		assert(instance.helloWorld().equals("I am a org.smcql.compiler.emp.generated.EmpJniDemo!"));
		
	}
	
	
	// tests linker access to pqxx
	public void testCount() throws Exception {
		String className = "org.smcql.compiler.emp.generated.Count";
		EmpBuilder builder = new EmpBuilder(className);
		builder.compile();
		EmpProgram instance = builder.getClass(1, 54321);
		assert(instance != null);
		// verify that it loads and is runnable
		String observedClass = instance.helloWorld();
		String expectedClass = "I am a org.smcql.compiler.emp.generated.Count!";
		assertEquals(observedClass, expectedClass);
		
	}

}
