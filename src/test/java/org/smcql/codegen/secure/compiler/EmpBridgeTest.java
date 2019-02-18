package org.smcql.codegen.secure.compiler;

import org.smcql.BaseTest;
import org.smcql.codegen.smc.compiler.DynamicCompiler;
import org.smcql.codegen.smc.compiler.emp.EmpProgram;
import org.smcql.codegen.smc.compiler.emp.ClassPathBuilder;
import org.smcql.codegen.smc.compiler.emp.EmpCompiler;
import org.smcql.util.CommandOutput;
import org.smcql.util.Utilities;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringUtils;

public class EmpBridgeTest extends BaseTest {
	protected void setUp() throws Exception {
		String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.localhost";
		System.setProperty("smcql.setup", setupFile);

	}
	

/*	private List<String> getBuildCommand(String srcFile, ClassPathBuilder classLoader) throws IOException {
		   ArrayList<String> command = new ArrayList<String>();
		   command.add("javac");
		   command.add("-cp");
		   
		   String paths = System.getProperty("java.class.path");
		   
		   String[] classPaths = classLoader.getPaths();
		   paths += paths + String.join(File.pathSeparator, classPaths);
		   
		   
		   command.add(paths);
		   command.add(srcFile);
		   
		   return command;       
		   
	}*/
	
	public void testCount() throws Exception {
		
		EmpCompiler compiler = new EmpCompiler("Count");
		int exitCode = compiler.compile();
		assertEquals(0, exitCode); //  it built successfully
		
		EmpProgram theProgram = EmpCompiler.loadClass("Count", 1, 54321);

		assert(theProgram != null); // we can load it
		
	}

}
