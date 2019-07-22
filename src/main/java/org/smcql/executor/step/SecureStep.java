package org.smcql.executor.step;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.smcql.codegen.CodeGenerator;
import org.smcql.codegen.smc.operator.support.UnionMethod;
import org.smcql.type.SecureRelRecordType;
import org.smcql.executor.config.RunConfig;
import org.smcql.executor.smc.OperatorExecution;
import org.smcql.plan.operator.Operator;

public class SecureStep implements ExecutionStep, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 5928302082757391973L;
	transient CodeGenerator codeGenerator;
	transient Operator srcOperator;
	ExecutionStep parent;
	List<ExecutionStep> children;
	RunConfig runConf;
	boolean visited = false;
	
	// hold on to state for serialization 
	boolean isMerge = false;
	OperatorExecution exec;
	
	public SecureStep(CodeGenerator cg, Operator op, RunConfig r, ExecutionStep lhsChild, ExecutionStep rhsChild) throws Exception {
		codeGenerator = cg;
		srcOperator = op;
		runConf = r;
		
		children = new ArrayList<ExecutionStep>();
		children.add(lhsChild);
		if(rhsChild != null)
			children.add(rhsChild);
		
		exec = new OperatorExecution(this);

		
		// needed to avoid serializing whole code generator
		if(cg instanceof UnionMethod)  {
			isMerge = true;
			exec.setSourceSQL(((UnionMethod) cg).getSourceSQL());
		}
	}
	
	
	@SuppressWarnings("unlikely-arg-type")
	@Override
	public String generate() throws Exception {
		return codeGenerator.generate().get(0);
	}

	@Override
	public OperatorExecution getExec() {
		return exec;
	}
	
	@Override
	public String getPackageName() {
		return codeGenerator.getPackageName();
	}

	@Override
	public ExecutionStep getParent() {
		return parent;
	}
	
	public void setParent(ExecutionStep p) {
		parent = p;
	}

	@Override
	public ExecutionStep getChild(int idx) {
		return (children.size() > idx) ? children.get(idx) : null;
		
	}

	@Override
	public SecureRelRecordType getInSchema() {
		return codeGenerator.getInSchema();
	}

	@Override
	public SecureRelRecordType getSchema() {
		return codeGenerator.getSchema();
	}

	@Override
	public RunConfig getRunConfig() {
		return runConf;
	}

	@Override
	public Operator getSourceOperator() {
		return srcOperator;
	}

	
		

	@Override
	public CodeGenerator getCodeGenerator() {
		return codeGenerator;
	}

	@Override
	public List<ExecutionStep> getChildren() {
		return children;
	}

	@Override
	public boolean visited() {
		return visited;
	}

	@Override
	public void visit() {
		 visited = true;
	}

	@Override
	public void setHostname(String host) {
		runConf.host = host;
		
	}


	@Override
	public SecureRelRecordType getSchema(boolean forSecureLeaf) {
		return getSchema();
	}

	public String toString() {
		return codeGenerator.toString();
	}

	@Override
	public String printTree() {
		return appendOperator(this, new String(), "");
	}
	
	private String appendOperator(ExecutionStep step, String src, String linePrefix) {
		String opString = step.toString();
		if (step instanceof SecureStep && ((SecureStep) step).isMerge)  // TODO: extend to take into account slicing
			opString = "LogicalMerge-DistributedOblivious"; 
		src += linePrefix + opString + "\n";
		linePrefix += "    ";
		for(ExecutionStep child : step.getChildren()) {
			src = appendOperator(child, src, linePrefix);
		}
		return src;
	}


	@Override
	public String getFunctionName() {
		return codeGenerator.getFunctionName();
	}
	
	public boolean isUnion() {
		return isMerge;
	}
}
