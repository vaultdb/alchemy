package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import org.smcql.codegen.CodeGenerator;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.plan.operator.Operator;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.Utilities;

// setup for emp code
public class SecurePreamble extends SecureOperator {

	/**
	 * Generate preamble for emp code
	 * 
	 */
	private static final long serialVersionUID = 1L;

	
	Vector<String> sqlStatements = null;
	
	public SecurePreamble(Operator o) throws Exception {
		super(o);  // store op in planNode
	}
	
	
	public void setSqlStatements(Map<ExecutionStep, String> sqlSrc) {
		sqlStatements = new Vector<String>(sqlSrc.values());
		
	}
	
	
	@Override
	public Map<String, String> generate() throws Exception  {
		Map<String, String> variables = new HashMap<String, String>();	
		assert(sqlStatements != null);
		
		String queries = new String();
		for(String sql : sqlStatements) {
			queries = queries +  sql + "\n";
		}
		
		variables.put("srcSQL", queries);
		String alice = ConnectionManager.getInstance().getConnectionString(0);
		String bob = ConnectionManager.getInstance().getConnectionString(1);
		
		variables.put("aliceConnectionString", alice);
		variables.put("bobConnectionString", bob);
		
		Map<String, String> result = new HashMap<String, String>();
		//result.put(getPackageName(), CodeGenUtils.generateFromTemplate("aggregate/singular/full/count.txt", variables));
		result.put("preamble", CodeGenUtils.generateFromTemplate("util/preamble.txt", variables));
		return result;
	}
	

}
