package org.vaultdb.codegen.smc.operator;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.vaultdb.codegen.CodeGenerator;
import org.vaultdb.codegen.smc.operator.support.ProcessingStep;
import org.vaultdb.codegen.smc.operator.support.SortMethod;
import org.vaultdb.executor.config.ExecutionMode;
import org.vaultdb.executor.step.ExecutionStep;
import org.vaultdb.plan.operator.Filter;
import org.vaultdb.plan.operator.Join;
import org.vaultdb.plan.operator.Operator;
import org.vaultdb.plan.operator.Project;
import org.vaultdb.plan.operator.Sort;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.CodeGenUtils;
import org.vaultdb.util.RexNodeUtilities;

public class SecureOperator implements CodeGenerator, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 4748853061046387638L;
	protected Operator planNode;
	protected String structName;
	protected String generatedCode = null;
	
	// TODO: convert filter into a list of rexnodes
	protected List<Filter> filters;
	
	protected List<Project> projects;
	protected List<ExecutionStep> merges;
	
	protected List<ProcessingStep> processingSteps;
	
	// not equal to Operator.children b/c of projects and filters getting folded into code gen for larger op
	protected List<SecureOperator> children;

	
	public SecureOperator(Operator o) throws Exception {
		planNode = o;
		if(planNode != null) { // accommodate generating preamble and main for emp
			children = new ArrayList<SecureOperator>();
		
			for (Operator child : planNode.getChildren()) {
				SecureOperator secureChild = child.getSecureOperator();
				if (secureChild != null)
					children.add(secureChild);
			}
		
			planNode.setSecureOperator(this);
			filters = new ArrayList<Filter>();
			projects = new ArrayList<Project>();
			merges = new ArrayList<ExecutionStep>();
			processingSteps = new ArrayList<ProcessingStep>();
		}	
	}
	

	public String generate() throws Exception {
		return new String();
	}
	
	
	
	public SecureRelRecordType getSchema() {
		if(!projects.isEmpty())
			return projects.get(0).getSchema();
		if (!merges.isEmpty())
			return merges.get(0).getCodeGenerator().getSchema();
		
		return planNode.getSchema();
	}

	
	
	public Operator getPlanNode() {
		return planNode;
	}
	
	public void addProject(Project aProject) {
		aProject.setSecureOperator(this);
		projects.add(aProject);
	}
	
	
	public void addFilter(Filter aFilter) {
		aFilter.setSecureOperator(this);
		filters.add(aFilter);
	}
	
	public void addProcessingStep(ProcessingStep step) {
		processingSteps.add(step);
	}
	
	public List<ProcessingStep> getProcessingSteps() {
		return processingSteps;
	}
	
	public List<ExecutionStep> getMerges() {
		return merges;
	}

	public void setMerges(List<ExecutionStep> merges) {
		this.merges = merges;
	}
	
	protected Operator getCorrectChild(Operator child, int mergeIndex) {
	    while (child instanceof Project || child instanceof Filter) {
	    	child = child.getChild(0); 
	    	if (!merges.isEmpty() && merges.get(mergeIndex).getSourceOperator().isSplittable()) {
	    		ExecutionStep m = merges.get(mergeIndex);
	    		child = m.getChild(0).getSourceOperator();
	    	}
	    		
	    }
	    
	    return child;
	}
	
	private String rewriteForJoin(String condition, String srcName) {
		String result = condition;
		boolean right = false;
		while (true) {
			int i = result.indexOf(srcName);
			if (i < 0)
				break;
			
			String replacement = (right) ? "rTuple" : "lTuple";
			result = result.replaceFirst(srcName, replacement);
			
			right = !right;
		}
		result = result.replace("true & ", "");
		return result;
	}
	
	private void handleFilters(Map<String, String> variables) {
		if (this.filters.isEmpty()) {
			variables.put("filterCond", "Bit(1, PUBLIC)");
		} else {
			String condition = "";
			int index = 0;
			String srcName = "tuple";
			for (Filter f : this.filters) {
				String filterCond = "(" + RexNodeUtilities.flattenFilter(f, srcName, Integer.parseInt(variables.get("sSize"))) + ")";
				// put it in CNF earlier
				condition += (index == 0) ? filterCond : " & " + filterCond;
				index++;
			}
			
			if (planNode instanceof Join)
				condition = rewriteForJoin(condition, srcName);
			
			
			variables.put("filterCond", condition);
		}		
	}
	
	private void handleProjects(Map<String, String> variables, SecureRelRecordType childSchema) throws Exception {
		if(!projects.isEmpty()) {
			String projectVars = RexNodeUtilities.flattenProjection(projects.get(0), "srcTuple", "dstTuple", childSchema.size());
			variables.put("writeDst", projectVars);
		}
		else {
			String writeDstContent = CodeGenUtils.writeFields(childSchema, "srcTuple", "dstTuple");
			variables.put("writeDst", writeDstContent);
		}
	}
	
	public Map<String, String> baseVariables() throws Exception {
		// variables: size, sortKeySize signal, fid, bitmask
		Map<String, String> variables = new HashMap<String, String>();
	   
		// tuple size in bits
		String tupleSize = Integer.toString(planNode.getSchema(true).size() + 1); // +1 for dummy tag
		variables.put("size", tupleSize);
		
		// for ops with different schemas between input and output
		// overridden by Join
		Operator child = getCorrectChild(planNode.getChild(0), 0);
		SecureRelRecordType childSchema = (merges.isEmpty()) ? child.getSchema(true) : merges.get(0).getCodeGenerator().getSchema();
		String srcSize = Integer.toString(childSchema.size() + 1); // + 1 for dummyTag
		String dstSize = projects.isEmpty() ? tupleSize : Integer.toString(projects.get(0).getSchema().size() + 1);
		
		variables.put("sSize", srcSize);
		variables.put("dSize", dstSize);

		
		handleFilters(variables); 
		handleProjects(variables, childSchema);
		
		// TODO: add function name here
		variables.put("functionName", planNode.getFunctionName());
		variables.put("packageName", planNode.getPackageName());
		
		if(!(planNode instanceof Sort) && !(planNode instanceof Join)) {
			if(child == null || children.isEmpty() || this.sharesComputeOrder(children.get(0))) {
				// no sort since it will either be handled by the SQL writer or by the child operator
				variables.put("sortInput", "");  // calls sort method
				variables.put("sortMethod", ""); // declares sort method
			}	
			else {
				SortMethod sm = null;
				if(planNode.isSplittable()) {
					sm = new SortMethod(planNode, planNode.secureComputeOrder());
				}
				else { 
					sm = new SortMethod(child, planNode.secureComputeOrder());
				}
				
				try {
					variables.put("sortInput", sm.sortInvocation("v", "aPrime"));
					variables.put("sortMethod", sm.sorter());
				} catch (Exception e) {
					variables.put("sortInput", "");
					variables.put("sortMethod", "");
				}
				
			
			}
		}
			
		
		return variables;

	}


	
	public boolean sharesComputeOrder(SecureOperator op) {
		List<SecureRelDataTypeField> opOrder = op.getPlanNode().secureComputeOrder();
		List<SecureRelDataTypeField> myOrder = planNode.secureComputeOrder();
		
		if(opOrder.equals(myOrder)) {
			return true;
		}
		return false;
		
	}
	
	




	@Override
	public String getPackageName() {
		return planNode.getPackageName();
	}
	
	@Override
	public String getFunctionName() {
		return planNode.getFunctionName();
	}
	

	@Override
	public SecureRelRecordType getInSchema() {
		return planNode.getInSchema();
	}



	
	@Override
	 public String destFilename(ExecutionMode e) {
		return planNode.destFilename(e);
	}

	
	



	@Override
	public String generate(boolean asSecureLeaf) throws Exception {
		return this.generate();
	}



	@Override
	public SecureRelRecordType getSchema(boolean asSecureLeaf) {
		return getSchema();
	}





    @Override
    public String toString() {
    	return planNode.toString();
    }

	

	

	


	
	
	
}
