package org.vaultdb.codegen.smc.operator.support;

import java.io.IOException;
import java.io.Serializable;
import java.util.Map;

import org.vaultdb.util.CodeGenUtils;

public class ProcessingStep implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 5248734443859082620L;
	private String templatePath;
	private String processName;
	private byte[] byteCode;
	
	public ProcessingStep(String templatePath, String processName) {
		this.templatePath = templatePath;
		this.processName = processName;
		this.setByteCode(null);
	}
	
	public String generate(Map<String, String> variables) throws IOException {
		String code = null;
		variables.put("processName", processName);
		code = CodeGenUtils.generateFromTemplate(templatePath, variables);
		
		return code;
	}
	
	public String getProcessName() {
		return processName;
	}

	public byte[] getByteCode() {
		return byteCode;
	}

	public void setByteCode(byte[] byteCode) {
		this.byteCode = byteCode;
	}
}
