package org.smcql.codegen.emp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.codegen.operators.MergeNode;
import org.smcql.util.CodeGenUtils;

public class EmpGenerator {
	
	public static String getEmpCode(CodeGenNode node) throws Exception {
		Map<String, String> variables = new HashMap<String, String>();
		List<String> srcSQL = new ArrayList<String>();
		List<String> functions = new ArrayList<String>();
		
		//constants
		variables.put("db_name_1", "healthlnk_site1");
		variables.put("db_name_2", "healthlnk_site2");
		variables.put("user", "smcql");
		variables.put("db_host_addr", "10.0.2.2");
		variables.put("db_port", "5432");
		variables.put("row_size", "320");
		variables.put("limit", "10");
		variables.put("col_length_0", "256");
		variables.put("col_length_1", "64");
		
		//traverse tree
		getCode(node, functions, srcSQL);
		
		//variables
		for (String src : srcSQL) {
			variables.put("src_sql", src);
		}
		
		//functions
		//TODO: make sure that projects are taken care of first
		String f = "\n    ";
		for (String fcn : functions) {
			f += fcn + "\n    ";
		}
		variables.put("functions", f);
		
		//generate
		String result = CodeGenUtils.generateFromTemplate("emp.cpp", variables);
		return result;
	}
	
	
	private static void getCode(CodeGenNode node, List<String> functions, List<String> srcSQL) throws Exception {
		if (node.isPublic()) {
			srcSQL.add(node.generateSQL().replace("\n", " "));
			return;
		}
	
		for (CodeGenNode child : node.getChildren()) 
			getCode(child, functions, srcSQL);
		
		functions.add(getFunction(node));
	}
	
	private static String getFunction(CodeGenNode node) throws Exception {
		String opName = (node instanceof MergeNode) ? "Merge" : node.getOperators().get(0).getOpName();
		String result = "";
		
		//TODO: add more operators
		switch (opName) {
			case "Sort": 
				result = "op_sort(res);";
				break;
			case "Aggregate": 
				result = "op_aggregate(res);";
				break;
			case "Merge":
				result = "Data *res = op_merge(input, row_size, alice_size, bob_size, party);";
				break;
			default: 
				throw new Exception("Unsupported operator " + opName);
			
		}
		
		return result;
	}
}
