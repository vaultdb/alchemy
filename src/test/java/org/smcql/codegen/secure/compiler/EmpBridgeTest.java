package org.smcql.codegen.secure.compiler;

import org.smcql.BaseTest;

import org.smcql.codegen.smc.DynamicCompiler;
import org.smcql.codegen.smc.EmpProgram;
import org.smcql.util.Utilities;
import org.bytedeco.javacpp.tools.Builder;

public class EmpBridgeTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}

	public void testCount() throws Exception {
		String srcFile = Utilities.getSMCQLRoot() + "/src/test/java/org/smcql/codegen/secure/compiler/inputs/Count.java";
//		String srcFile = "inputs/Count.java";
//		String srcFile =  "/src/test/java/org/smcql/codegen/secure/compiler/inputs/Count.java";

		System.out.println("Source file: " + srcFile);
		
		String[] args = new String[2];
		args[0] = "-nodelete";
		args[1] = srcFile;
		
		Builder.main(args);
		
		DynamicCompiler.compileJava(srcFile, "org.smcql.codegen.secure.compiler.inputs.Count");
		EmpProgram theProgram = DynamicCompiler.loadClass("org.smcql.codegen.secure.compiler.inputs.Count", 1, 54321);
		boolean[] output = theProgram.runProgram();
	}

}
