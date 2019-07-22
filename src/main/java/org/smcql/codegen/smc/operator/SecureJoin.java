package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.calcite.rex.RexNode;
import org.smcql.codegen.smc.operator.support.ProcessingStep;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.operator.Join;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.RexNodeUtilities;

public class SecureJoin extends SecureOperator {

	/**
	 * 
	 */
	private static final long serialVersionUID = 7379127156933722928L;

	Logger logger;
    SecureRelRecordType schema;
	
	public SecureJoin(Operator o) throws Exception {
		super(o);
		
		assert(o instanceof Join);
		
		
		        
        SystemConfiguration config = SystemConfiguration.getInstance();
    	logger = config.getLogger();

        schema = o.getSchema();
       
	}


	
	@Override
	public String generate() throws Exception  {
		Map<String, String> variables = baseVariables();		

		Join join = (Join) planNode;
		int lSize = join.getChild(0).getSchema().size();
		int rSize = join.getChild(1).getSchema().size();
		String lSizeStr = Integer.toString(lSize);
		String rSizeStr = Integer.toString(rSize);
		int srcTupleSize = lSize + rSize;
		
		variables.put("lSize", lSizeStr);
		variables.put("rSize", rSizeStr);
		
		RexNode selectionCriteria = join.getCondition();
		
		String selectionExpr = RexNodeUtilities.flattenForSmc(selectionCriteria, schema, "srcTuple", srcTupleSize);
		variables.put("filterCond", selectionExpr);
		
		if(selectionCriteria == null) {
			generatedCode =  CodeGenUtils.generateFromTemplate("join/cross.txt", variables);	
		}
		else {	
			generatedCode =  CodeGenUtils.generateFromTemplate("join/simple.txt", variables);
		}
		
		return generatedCode;
	}
	
	
 	

}
