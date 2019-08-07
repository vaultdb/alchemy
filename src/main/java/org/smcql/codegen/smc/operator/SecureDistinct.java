package org.smcql.codegen.smc.operator;

import java.util.Map;

import org.smcql.config.SystemConfiguration;
import org.smcql.plan.operator.Operator;
import org.vaultdb.util.CodeGenUtils;


public class SecureDistinct extends SecureOperator{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1614222159963050329L;

	public SecureDistinct(Operator o) throws Exception {
		super(o);
		
	}
	
	@Override
	public String generate() throws Exception  {
		Map<String, String> variables = baseVariables();
		
		String generatedCode = null;

		if(planNode.getExecutionMode().sliced && SystemConfiguration.getInstance().slicingEnabled()) {
			generatedCode = CodeGenUtils.generateFromTemplate("distinct/sliced.txt", variables);
		}
		else {
			generatedCode = CodeGenUtils.generateFromTemplate("distinct/simple.txt", variables);
		}

		return generatedCode;
	}

}
