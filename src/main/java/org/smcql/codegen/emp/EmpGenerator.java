package org.smcql.codegen.emp;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.codegen.operators.MergeNode;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.CodeGenUtils;

public class EmpGenerator {
	
	public static String getEmpCode(CodeGenNode node) throws Exception {
		Map<String, String> variables = new HashMap<String, String>();
		List<String> srcSQL = new ArrayList<String>();
		List<String> functions = new ArrayList<String>();
		
		//constants
		
		// db1 config: 
		WorkerConfiguration alice = ConnectionManager.getInstance().getWorkerConfigurations().get(0);
		WorkerConfiguration bob = ConnectionManager.getInstance().getWorkerConfigurations().get(1);
		
		
	    variables.put("db_name_1", alice.dbName);
		variables.put("db_name_2", bob.dbName);
		variables.put("user", alice.user); // TODO: incorporate multi-host users/passes/IP addresses
		variables.put("db_host_addr", alice.hostname);
		variables.put("db_host_addr", "10.0.2.2");
		variables.put("db_port", Integer.toString(alice.dbPort));
		variables.put("row_size", "320");  // This should be variable depending on the operator's schema, will change from op to op
		
		//TODO: eliminate hard coding
		variables.put("limit", "10");
		variables.put("col_length_0", "256");
		variables.put("col_length_1", "64");
		
		//traverse tree
		getCode(node, functions, srcSQL, "res_0");
		
		//source SQL
		String s = "";
		for (int i=0; i<srcSQL.size(); i++) {
			s += "string sql_query_" + i + " = \"" + srcSQL.get(i) + "\";\n";
		}
		variables.put("src_sql", s);
		
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
	
	
	private static void getCode(CodeGenNode node, List<String> functions, List<String> srcSQL, String varName) throws Exception {
		if (node.isPublic()) {
			srcSQL.add(node.generateSQL().replace("\n", " "));
			return;
		}
	
		for (int i=0; i< node.getChildren().size(); i++) {
			int index = Character.getNumericValue(varName.charAt(varName.length()-1)) + i;
			String name = varName.substring(0, varName.length()-1) + index;
			getCode(node.getChildren().get(i), functions, srcSQL, name);
		}
		
		functions.add(getFunction(node, varName));
	}
	
	private static String getCols(CodeGenNode node, String opName) {
		if (opName.equals("Join")) {
			return ""; //TODO: implement this
		}
		
		String result = "vector<int> input_col_lengths{";
		SecureRelRecordType inSchema = node.getOperators().get(0).getInSchema();
		for (int i=0; i<inSchema.getAttributes().size(); i++) {
			if (i!=0)
				result += ", ";
			
			result += inSchema.getAttributes().get(i).size();
		}
		result += "};\n";
		
		
		result += "    vector<int> output_col_lengths{";
		SecureRelRecordType outSchema = node.getOperators().get(0).getSchema();
		for (int i=0; i<outSchema.getAttributes().size(); i++) {
			if (i!=0)
				result += ", ";
			
			result += outSchema.getAttributes().get(i).size();
		}
		result += "};\n";
		
		return result;
	}
	
	private static String getFunction(CodeGenNode node, String varName) throws Exception {
		String opName = (node instanceof MergeNode) ? "Merge" : node.getOperators().get(0).getOpName();
		String result = getCols(node, opName);
		
		switch (opName) {
			case "Sort":
				result += "    op_sort(" + varName + ", input_col_lengths, output_col_lengths);";
				break;
			case "Aggregate": 
				result += "    op_aggregate(" + varName + ", input_col_lengths, output_col_lengths);";
				break;
			case "WindowAggregate":
				result += "    op_window_aggregate(" + varName + ", input_col_lengths, output_col_lengths);";
				break;
			case "Distinct":
				result += "    op_distinct(" + varName + ", input_col_lengths, output_col_lengths);";
				break;
			case "Join":
				//TODO: handle self join
				String rightName = varName.substring(0, varName.length()-1);
				rightName += (Character.getNumericValue(varName.charAt(varName.length()-1)) + 1);
				result = varName + " = op_join(" + varName + ", " + rightName +");";
				break;
			case "Merge":
				int index = Character.getNumericValue(varName.charAt(varName.length()-1));
				result += "    Data *" + varName + " = op_merge(sql_query_" + index + ", output_col_lengths, party, io);";
				break;
			default: 
				throw new Exception("Unsupported operator " + opName);
			
		}
		
		return result;
	}
}
