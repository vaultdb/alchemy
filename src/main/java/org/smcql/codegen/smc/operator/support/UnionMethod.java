package org.smcql.codegen.smc.operator.support;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;

import java.util.List;
import java.util.Map;

import org.apache.calcite.util.Pair;
import org.apache.commons.lang3.StringUtils;
import org.smcql.codegen.CodeGenerator;
import org.smcql.compiler.DynamicCompiler;
import org.smcql.executor.config.RunConfig.ExecutionMode;
import org.smcql.executor.step.ExecutionStep;
import org.smcql.executor.step.PlaintextStep;
import org.smcql.plan.operator.Operator;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;
import org.smcql.util.Utilities;

// takes in the sorted input of Alice and Bob 
// and creates a single joint set of tuples for query execution
public class UnionMethod implements CodeGenerator, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 5083349178327948258L;
	// if smc leaf is splittable, operate on secure leaf
	// else operates on the *child* of the SMC op
	Operator src;
	String packageName;
	String generatedCode = null;
	private String srcSQL;
	private SecureRelRecordType schema;
	private List<SecureRelDataTypeField> orderKey;
	ExecutionStep childStep;
	private String functionName;
	
	public UnionMethod(Operator op, ExecutionStep child, List<SecureRelDataTypeField> orderBy) throws Exception{
		src = op;
		orderKey = orderBy;
		childStep = child;
		
		srcSQL = (child instanceof PlaintextStep) ? child.generate() : null;
		System.out.println("UnionMethod srcSQL: " + srcSQL);
		
		packageName = child.getPackageName() + ".merge";
		functionName = child.getFunctionName() + "Merge";
		if (srcSQL != null) {
			schema = Utilities.getOutSchemaFromSql(srcSQL);
		} else {
			schema = child.getSourceOperator().getSchema(true);
		}
	}
	// for use in testing
	public UnionMethod(String sql, String pack) throws Exception {
		packageName = pack;
		srcSQL = sql;
		orderKey = new ArrayList<SecureRelDataTypeField>();
		schema = Utilities.getOutSchemaFromSql(sql);
		packageName = pack + ".merge";
	}
	
	public void addOrderByAttribute(String attrName) {
		SecureRelDataTypeField attr  = schema.getAttribute(attrName);
		orderKey.add(attr);
			
	}
	
	@Override
	public Map<String, String> generate() throws Exception {
		int size = schema.size();

		Map<String, String> variables = new HashMap<String, String>();
		//variables.put("lessThan", generateLessThan());
		variables.put("size", Integer.toString(size));
		variables.put("packageName", packageName);
		variables.put("functionName", functionName);
		String sql =  srcSQL.replace('\n', ' ');
		variables.put("sql", sql);
		
		
		
		String unionFile = "union/ordered.txt";
		
		if(orderKey.isEmpty()){
			unionFile = "union/unordered.txt";
		}
		else {
			//TODO: add support for composite merge key
			Pair<Integer, Integer> schemaPos = CodeGenUtils.getSchemaPosition(schema.getAttributes(), orderKey.get(0));
			variables.put("keyLength", Integer.toString(schemaPos.getKey()));
			variables.put("keyPos", Integer.toString(schemaPos.getValue()));
		}
		
		generatedCode = CodeGenUtils.generateFromTemplate(unionFile, variables);
		Map<String, String> result = new HashMap<String, String>();
		result.put(packageName, generatedCode);
		return result;
	}
	
	
	@Override
	public String getPackageName() {
		return packageName;
	}

	@Override
	public String getFunctionName() {
		return functionName;
	}

	
	@Override
	public SecureRelRecordType getInSchema() {
		return schema;
	}

	@Override
	public SecureRelRecordType getSchema() {
		return schema;
	}

	@Override
	public SecureRelRecordType getSchema(boolean generateForSMC) {
		return schema;
	}
	
	public String getSourceSQL() {
		return srcSQL;
	}

	public void setSourceSQL(String srcSQL) {
		this.srcSQL = srcSQL;
	}
	
	@Override
	public String destFilename(ExecutionMode e) {
		String base = childStep.getCodeGenerator().destFilename(e);
		base = base.substring(0, base.length() - ".lcc".length());
		return base + "_merge.lcc";
	}
	
	@Override
	public void compileIt() throws Exception {
		Map<String, String> code = generate();
		
		for (String n : code.keySet())
			DynamicCompiler.compileOblivLang(code.get(n), n);
	}
	
	@Override
	public Map<String, String> generate(boolean asSecureLeaf) throws Exception {
		return new HashMap<String, String>();
	}
	





	

}
