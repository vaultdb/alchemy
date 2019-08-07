package org.smcql.codegen;

import java.util.Map;

import org.smcql.executor.config.ExecutionMode;
import org.vaultdb.type.SecureRelRecordType;


public interface CodeGenerator {

	String generate() throws Exception;

	
	String generate(boolean asSecureLeaf) throws Exception;

	public String getPackageName();
	
	
	public SecureRelRecordType getInSchema();
	
	
	public String destFilename(ExecutionMode e);
	
	//public void compileIt() throws Exception;

	public SecureRelRecordType getSchema();



	public SecureRelRecordType getSchema(boolean asSecureLeaf);


	String getFunctionName();

	
}
