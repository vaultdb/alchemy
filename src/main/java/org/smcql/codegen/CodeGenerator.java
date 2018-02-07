package org.smcql.codegen;

import java.util.Map;

import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.type.SecureRelRecordType;


public interface CodeGenerator {

	Map<String, String> generate() throws Exception;

	
	Map<String, String> generate(boolean asSecureLeaf) throws Exception;

	public String getPackageName();
	
	
	public SecureRelRecordType getInSchema();
	
	
	public String destFilename(ExecutionMode e);
	
	public void compileIt() throws Exception;

	public SecureRelRecordType getSchema();



	public SecureRelRecordType getSchema(boolean asSecureLeaf);

	
}
