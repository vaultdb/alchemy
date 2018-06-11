package org.smcql.codegen.emp;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.smcql.codegen.operators.CodeGenNode;
import org.smcql.util.CodeGenUtils;

public class EmpGenerator {
	
	
	public static String getEmpCode(CodeGenNode node) throws IOException {
		Map<String, String> variables = new HashMap<String, String>();
		
		variables.put("db_name_1", "healthlnk_site1");
		variables.put("db_name_2", "healthlnk_site2");
		variables.put("user", "smcql");
		variables.put("db_host_addr", "10.0.2.2");
		variables.put("db_port", "5432");
		variables.put("src_sql", "SELECT major_icd9, COUNT(*) AS cnt "
								+ "FROM cdiff_cohort_diagnoses "
								+ "GROUP BY major_icd9 "
								+ "ORDER BY major_icd9");
		variables.put("row_size", "320");
		variables.put("limit", "10");
		variables.put("col_length_0", "256");
		variables.put("col_length_1", "64");
		
		String result = CodeGenUtils.generateFromTemplate("emp.txt", variables);
		return result;
	}
}
