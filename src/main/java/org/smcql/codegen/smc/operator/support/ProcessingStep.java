package org.smcql.codegen.smc.operator.support;

import java.io.IOException;
import java.util.Map;

import org.smcql.util.CodeGenUtils;

public class ProcessingStep {
	private String templatePath;
	
	public ProcessingStep(String templatePath) {
		this.templatePath = templatePath;
	}
	
	public String generate(Map<String, String> variables) throws IOException {
		String code = null;
		code = CodeGenUtils.generateFromTemplate(templatePath, variables);
		
		return code;
	}
}
