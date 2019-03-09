package org.smcql.compiler;

import org.smcql.BaseTest;
import org.smcql.compiler.emp.EmpBuilder;
import org.smcql.compiler.emp.EmpCompiler;
import org.smcql.compiler.emp.EmpParty;
import org.smcql.compiler.emp.EmpProgram;
import org.smcql.db.data.QueryTable;
import org.smcql.util.Utilities;


public class EmpBuilderTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}
	



	// tests emp-jni link with minimal dependencies
	public void testEmpJniDemo() throws Exception {
		String className = "org.smcql.compiler.emp.generated.EmpJniDemo";
		EmpBuilder builder = new EmpBuilder();
		builder.compile(className, true);
		EmpProgram instance = builder.getClass(className, 1, 54321);
		assert(instance != null);
		// verify that it loads and is runnable
		assert(instance.helloWorld().equals("I am an EmpJniDemo!"));
		
	}
	
	
	// tests linker access to pqxx
	public void testCount() throws Exception {
		String className = "org.smcql.compiler.emp.generated.Count";
		EmpBuilder builder = new EmpBuilder();
		builder.compile(className, true);
		EmpProgram instance = builder.getClass(className, 1, 54321);
		assert(instance != null);
		// verify that it loads and is runnable
		String observedClass = instance.helloWorld();
		String expectedClass = "I am a org.smcql.compiler.emp.generated.Count!";
		assertEquals(observedClass, expectedClass);
		
	}

}
