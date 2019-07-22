package org.smcql.codegen.smc.operator;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.rel.RelFieldCollation;
import org.smcql.plan.operator.Operator;
import org.smcql.plan.operator.Sort;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.TypeMap;
import org.smcql.util.CodeGenUtils;

public class SecureSort extends SecureOperator {

	/**
	 * 
	 */
	private static final long serialVersionUID = -7834022673873444402L;
	private boolean keyed = false;
	
	public SecureSort(Operator o) throws Exception {
		super(o);
	}
	
	
	
	// pulls the sort keys out of the tuple
	// regardless of their order in the schema
	public static String extractKeyFunction(Operator s) throws Exception {

	
		TypeMap tmap = TypeMap.getInstance();
		List<SecureRelDataTypeField> sortKey = s.secureComputeOrder();
		int sortKeySize = tmap.getSize(sortKey);
		
		if(sortKey.size() < 1) {
			throw new Exception("No valid sort order for " + s.getOpName() + s.getOperatorId());
		}
		

		int dstOffset = 0;
		
		String ret = "Integer extractKey" + "(Integer src) {\n";
		ret += "    Integer dst = Integer(" + sortKeySize + ", 0, PUBLIC);\n\n";
		
		
		for(int i = 0; i < sortKey.size(); ++i) {
			SecureRelDataTypeField r = sortKey.get(i);
			int srcOffset = s.getSchema().getFieldOffset(r.getIndex());
			int writeSize = r.size();
			
			
			//String bitmask = s.getSchema().getInputRef(r, null);
			// memcpy(dst + writeStartIdx, src + 
			ret += CodeGenUtils.writeField("src", "dst", srcOffset, dstOffset, writeSize);
			dstOffset += r.size();
		}

		ret += "\n    return dst;\n }";
		
		
		return ret;

		
	}
	
	
	
	@Override
	public String generate() throws Exception  {
		
		// variables: size, sortKeySize signal, fid, bitmask
		Map<String, String> variables = baseVariables();

		assert(planNode instanceof Sort);
		Sort sort = (Sort) planNode;
		
		int limit = sort.getLimit();
		
		
		
		TypeMap tmap = TypeMap.getInstance();
		List<SecureRelDataTypeField> sortKey = sort.secureComputeOrder();
		
		RelFieldCollation.Direction sortDirection = sort.getSortDirection();
		int sig = sortDirection == RelFieldCollation.Direction.ASCENDING ? 1 : 0;
		String signal = Integer.toString(sig);
		variables.put("signal", signal);
		if(limit > -1) { 
			variables.put("limitDefine", "#define LIMIT $limit");
			variables.put("limitVar", "LIMIT");
			variables.put("limit", Integer.toString(limit));
			variables.put("runHead", "int"+variables.get("size")+"[LIMIT] run@n@m(int"+variables.get("size")+"[n] a, int"+variables.get("size")+"[m] b) { ");
		}
		else {
			variables.put("limitDefine", "");
			variables.put("limitVar", "n");
			variables.put("limit", "n");
			variables.put("runHead", "int"+variables.get("size")+"[n] run@n@m(int"+variables.get("size")+"[n] a, int"+variables.get("size")+"[m] b) { ");
		}
		
		
		String handleNulls = "";
		if(sig == 1) {
			handleNulls = "      // push the nulls to the end\n";
			handleNulls += "     bfor(fetch)(int i = 0; i < fetch; i=i+1) {\n";
			handleNulls += "             if(i < tupleCount) {\n";
			handleNulls += "                  toSort[i] = toSort[i + nulls];\n";
		    handleNulls += "              }\n";
		    handleNulls += "              else {\n";
            handleNulls += " 		      		  toSort[i] = 0;\n";
		    handleNulls += "                   }\n";
		    handleNulls += "        }\n\n";
			
		}
		variables.put("handleNulls", handleNulls);
		
		
		// plain sort
		if(planNode.getSchema().getSecureFieldList().equals(sortKey)) {
			generatedCode =  CodeGenUtils.generateFromTemplate("sort/simple.txt", variables);
			return generatedCode;
		}

		// add functionality for ORDER BY over part of tuple or different order of attributes
		keyed = true;
		String sortKeySize = Integer.toString(tmap.getSize(sortKey)); 
		variables.put("sortKeySize", sortKeySize);
		
		
		String extractKey = extractKeyFunction(sort);
		variables.put("extractKey", extractKey);
		
		String srcFile = "sort/keyed.txt";
		
		generatedCode = CodeGenUtils.generateFromTemplate(srcFile, variables);
		return generatedCode;
	}
	
}
