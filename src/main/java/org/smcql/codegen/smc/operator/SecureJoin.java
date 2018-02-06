package org.smcql.codegen.smc.operator;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.apache.calcite.rel.logical.LogicalJoin;
import org.smcql.codegen.smc.operator.support.ProcessingStep;
import org.smcql.plan.SecureRelNode;
import org.smcql.plan.operator.Filter;
import org.smcql.plan.operator.Join;
import org.smcql.plan.operator.Operator;
import org.smcql.util.CodeGenUtils;

public class SecureJoin extends SecureOperator{

	/**
	 * 
	 */
	private static final long serialVersionUID = 7379127156933722928L;

	public SecureJoin(Operator o) throws Exception {
		super(o);
		
		//Add any conditions as filters
		LogicalJoin j = (LogicalJoin) o.getSecureRelNode().getRelNode();
		LogicalFilter f = LogicalFilter.create(j.getInput(0), j.getCondition());
		
		SecureRelNode[] nodeChildren = Arrays.copyOf(o.getSecureRelNode().getChildren().toArray(), o.getSecureRelNode().getChildren().size(), SecureRelNode[].class);
		Operator[] opChildren = Arrays.copyOf(o.getChildren().toArray(), o.getChildren().size(), Operator[].class);
		
		Filter filter = new Filter(this.getPackageName(), new SecureRelNode(f, nodeChildren), opChildren, (Join) o);
		this.addFilter(filter);
		
		ProcessingStep trueSize = new ProcessingStep("util/dp_size.txt");
		processingSteps.add(trueSize);
	}

	@Override
	// implementing a standard NLJ so sorting is not tackled yet
	public List<String> generate() throws Exception  {
		Map<String, String> variables = baseVariables();		

		Join join = (Join) planNode;

		variables.put("lSize", variables.get("sSize"));
		variables.put("rSize", variables.get("sSize"));
				
		if(join.getCondition() == null) {
			generatedCode =  CodeGenUtils.generateFromTemplate("join/cross.txt", variables);	
		}
		else {	
			generatedCode =  CodeGenUtils.generateFromTemplate("join/simple.txt", variables);
		}
		
		List<String> result = new ArrayList<String>();
		result.add(generatedCode);
		return result;
	}
	
	
 	

}
