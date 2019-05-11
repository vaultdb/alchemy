package org.smcql.codegen.smc.operator;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.apache.calcite.rel.RelNode;
import org.apache.calcite.rel.core.RelFactories;
import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.apache.calcite.rex.RexNode;
import org.smcql.codegen.smc.operator.support.ProcessingStep;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Join;
import org.smcql.plan.operator.Operator;
import org.smcql.util.CodeGenUtils;

public class SecureJoin extends SecureOperator {

	/**
	 * 
	 */
	private static final long serialVersionUID = 7379127156933722928L;

	public SecureJoin(Operator o) throws Exception {
		super(o);
		
		assert(o instanceof Join);
		
		
		//Add any join conditions as filters
		LogicalJoin j = (LogicalJoin) o.getSecureRelNode().getRelNode();

		// it is getting stuck because we need a composite schema rather than just that of the first child
        RexNode selectionCriteria = j.getCondition();
        

		RelNode f =LogicalFilter.create(j, selectionCriteria);
		
		

		//SecureRelNode[] nodeChildren = Arrays.copyOf(o.getSecureRelNode().getChildren().toArray(), o.getSecureRelNode().getChildren().size(), SecureRelNode[].class);
		Operator[] opChildren = Arrays.copyOf(o.getChildren().toArray(), o.getChildren().size(), Operator[].class);
		SecureRelNode src = new SecureRelNode(f, o.getSecureRelNode());
		
		Filter filter = new Filter(this.getPackageName(), src, opChildren, (Join) o);
		this.addFilter(filter);
	}

	@Override
	public Map<String, String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();		

		Join join = (Join) planNode;
		String lSize = Integer.toString(join.getChild(0).getSchema().size());
		String rSize = Integer.toString(join.getChild(1).getSchema().size());

		variables.put("lSize", lSize);
		variables.put("rSize", rSize);
		//String dstSize = (projects.isEmpty()) ? variables.get("sSize") : Integer.toString(projects.get(0).getSchema().size());
		//variables.put("dSize", dstSize);
				
		if(join.getCondition() == null) {
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
