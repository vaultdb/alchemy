package org.vaultdb.codegen.plain;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import org.apache.calcite.plan.RelOptUtil;
import org.apache.calcite.rel.RelNode;
import org.apache.calcite.sql.SqlExplainLevel;
import org.apache.calcite.sql.pretty.SqlPrettyWriter;
import org.vaultdb.BaseTest;
import org.vaultdb.codegen.sql.SqlGenerator;


public class GenerateSqlTest  extends  BaseTest {
	
	protected void setUp() throws Exception {
		super.setUp();
	}
	
	public void testAsprinCount() throws Exception {
		 String expected =  "SELECT COUNT(DISTINCT mi_cohort_diagnoses.patient_id) AS rx_cnt\n"
				 + "FROM mi_cohort_diagnoses\n"
				 + "INNER JOIN mi_cohort_medications ON mi_cohort_diagnoses.patient_id = mi_cohort_medications.patient_id\n"
				 + "WHERE LOWER(mi_cohort_medications.medication) LIKE '%aspirin%' AND mi_cohort_diagnoses.icd9 LIKE '414%' AND mi_cohort_diagnoses.timestamp_ <= mi_cohort_medications.timestamp_";		 
		 runTest("aspirin-count", expected);		
	}
	
	
	public void testCDiff() throws Exception {
		String expected = 
				"SELECT DISTINCT t0.patient_id\n"
						 + "FROM (SELECT patient_id, EXTRACT(EPOCH FROM timestamp_) / 86400 AS day_, ROW_NUMBER() OVER (PARTITION BY patient_id ORDER BY timestamp_ ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS r\n"
						 + "FROM cdiff_cohort_diagnoses\n"
						 + "WHERE icd9 = '008.45') AS t0\n"
						 + "INNER JOIN (SELECT patient_id, EXTRACT(EPOCH FROM timestamp_) / 86400 AS day_, ROW_NUMBER() OVER (PARTITION BY patient_id ORDER BY timestamp_ ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS r\n"
						 + "FROM cdiff_cohort_diagnoses\n"
						 + "WHERE icd9 = '008.45') AS t2 ON t0.patient_id = t2.patient_id\n"
						 + "WHERE t2.day_ - t0.day_ >= 15 AND t2.day_ - t0.day_ <= 56 AND t0.r + 1 = t2.r";
		
		runTest("cdiff", expected);
	}

	public void testComorbidity() throws Exception {
		String expected =  "SELECT major_icd9, COUNT(*) AS cnt\n"
				+ "FROM cdiff_cohort_diagnoses\n"
				+ "GROUP BY major_icd9\n"
				+ "ORDER BY COUNT(*) DESC\n"
				+ "FETCH NEXT 10 ROWS ONLY";
		
		runTest("comorbidity", expected);
	}

	public void runTest(String testName, String expected) throws Exception {
		String sql = super.readSQL(testName);
		logger.info("Parsing:\n " + sql);
		runSqlTest(sql, expected);
		}

	
    public void runSqlTest(String sql, String canonicalized) throws Exception {
    	root = parser.parseSQL(sql);
		relRoot = parser.compile(root);
				
		

		logger.info("Operator tree:\n" + RelOptUtil.toString(relRoot.project(), SqlExplainLevel.ALL_ATTRIBUTES));
		generateSql();
		String sqlFromRel = rel2sql();
		
		assertEquals(canonicalized, sqlFromRel);

    }
    
    
// generate SQL from SqlNode
void generateSql() throws SQLException {
	SqlPrettyWriter writer = new SqlPrettyWriter(dialect);
	writer.setQuoteAllIdentifiers(false);
	writer.setAlwaysUseParentheses(false);

	root.unparse(writer, 0, 0);
				
	String sqlOut = writer.toSqlString().getSql();
	sqlOut = sqlOut.replace("\"", "");		
		
	System.out.println("Root is instance of " + root.getClass() + " kind: " + root.getKind());
	System.out.println("With sql parser: " + sqlOut);
			
}

// go from RelNode --> SQL
public String rel2sql() {
	String sqlOut =  SqlGenerator.getSql(relRoot.rel, dialect);
	System.out.println("Root sql " + sqlOut);
	return sqlOut;
}

// examine partial plans
public void walkTree() throws SQLException {
	
	List<RelNode> inputs = relRoot.project().getInputs();
	
	while(!inputs.isEmpty()) {
		List<RelNode> nextGeneration = new ArrayList<RelNode>();
		for(RelNode r : inputs) {
			System.out.println("For operator " + r.getDigest() + " have class " + r.getClass() + " and SQL:\n " + SqlGenerator.getSql(r, dialect));
			nextGeneration.addAll(r.getInputs());
		}
		inputs = nextGeneration;
	}
	
}

}
