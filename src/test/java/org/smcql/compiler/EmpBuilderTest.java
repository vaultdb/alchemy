package org.smcql.compiler;

import org.smcql.BaseTest;
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
	


	
	public void testCount() throws Exception {
		
		EmpParty theParty = new EmpParty(0);
		EmpCompiler compiler = new EmpCompiler("Count", theParty);
		int exitCode = compiler.compile();
		assertEquals(0, exitCode); //  it built successfully
		
		//Alice
		EmpProgram theProgram = compiler.loadClass();
		assert(theProgram != null); // we can load it
		
		//theProgram.runProgram();
		//boolean[] aliceOut = theProgram.getOutput();
		
		
		//TODO: Bob
		
		
		//TODO: xor and collect output (ex. EMPExecutor.getOutput)
		
	}

}
