package org.smcql.codegen.secure.compiler;

import org.smcql.BaseTest;
import org.smcql.codegen.smc.compiler.emp.EmpProgram;
import org.smcql.db.data.QueryTable;
import org.smcql.codegen.smc.compiler.emp.EmpCompiler;
import org.smcql.util.Utilities;


public class EmpBuilderTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}
	


	
	public void testCount() throws Exception {
		
		EmpCompiler compiler = new EmpCompiler("Count");
		int exitCode = compiler.compile();
		assertEquals(0, exitCode); //  it built successfully
		
		//Alice
		EmpProgram theProgram = EmpCompiler.loadClass("Count", 1, 54321);
		assert(theProgram != null); // we can load it
		
		theProgram.runProgram();
		boolean[] aliceOut = theProgram.getOutput();
		
		
		//TODO: Bob
		
		
		//TODO: xor and collect output (ex. EMPExecutor.getOutput)
		
	}

}
