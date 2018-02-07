package org.smcql.codegen.smc.operator.support;

import java.io.IOException;
import java.util.Map;

import org.smcql.util.CodeGenUtils;

public class ProcessingStep {
	private String templatePath;
	private String processName;
	
	public ProcessingStep(String templatePath, String processName) {
		this.templatePath = templatePath;
		this.processName = processName;
	}
	
	public String generate(Map<String, String> variables) throws IOException {
		String code = null;
		variables.put("processName", processName);
		code = CodeGenUtils.generateFromTemplate(templatePath, variables);
		
		return code;
	}
}
