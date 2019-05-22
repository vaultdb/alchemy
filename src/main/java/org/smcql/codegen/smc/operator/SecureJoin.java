package org.smcql.codegen.smc.operator;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.core.RelFactories;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rel.type.RelDataTypeField;
import org.apache.calcite.rex.RexCall;
import org.apache.calcite.rex.RexNode;
import org.apache.calcite.schema.SchemaPlus;
import org.apache.calcite.schema.Table;
import org.smcql.codegen.smc.operator.support.ProcessingStep;
import org.smcql.codegen.smc.operator.support.TypeCorrecter;
import org.smcql.config.SystemConfiguration;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Join;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.RexNodeUtilities;
import org.smcql.util.Utilities;

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
	public Map<String, String> generate() throws Exception  {
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
		
		Map<String, String> result = new HashMap<String, String>();
		result.put(getPackageName(), generatedCode);
		
		for (ProcessingStep p : processingSteps)
			result.put(getPackageName() + "." + p.getProcessName(), p.generate(variables));
		
		return result;
	}
	
	
 	

}
