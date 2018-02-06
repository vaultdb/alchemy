package org.smcql.codegen.smc.operator;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.plan.operator.Operator;
import org.smcql.util.CodeGenUtils;


public class SecureDistinct extends SecureOperator{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1614222159963050329L;

	public SecureDistinct(Operator o) throws Exception {
		super(o);
		
	}
	
	@Override
	public List<String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();
		
		String generatedCode = null;

		if(planNode.getExecutionMode() == ExecutionMode.Slice && SystemConfiguration.getInstance().getProperty("sliced-execution").equals("true")) {
			generatedCode = CodeGenUtils.generateFromTemplate("distinct/sliced.txt", variables);
		}
		else {
			generatedCode = CodeGenUtils.generateFromTemplate("distinct/simple.txt", variables);
		}

		List<String> result = new ArrayList<String>();
		result.add(generatedCode);
		return result;
	}

}
