package org.vaultdb.plan.execution.slice.statistics;

import java.util.List;

import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.plan.slice.SliceKeyDefinition;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

public class StatisticsCollector {
	
	// performed once per secure leaf
	public static SliceStatistics collect(SliceKeyDefinition s) throws Exception {	
		SecureRelDataTypeField keyField = s.getAttributes().get(0);		
		String attribute = keyField.getStoredAttribute();
		
		if (attribute == null)
			return new SliceStatistics(s);
		
		List<String> predicates = s.getFilters();
		
		if (predicates.size() == 0) {
			String table = keyField.getStoredTable();
			String query = "SELECT " + table + "." + attribute + ",site.id, COUNT(*) cnt FROM " + table + ", site GROUP BY " + table + "." + attribute + ", site.id";
			SecureRelRecordType schema = Utilities.getOutSchemaFromSql(query);
			return getResults(query, schema, s);
		}
		
		String query = "SELECT * FROM (\n";
		SecureRelRecordType schema = null;
		for (int i=0; i<predicates.size(); i++) {
			String predicate = predicates.get(i);
			String table = predicate.split("\\.")[0];
			
			String subQuery = "SELECT " + table + "." + attribute + ", site.id, COUNT(*) cnt\n"
					+ "FROM " + table + ", site\n";
			
			subQuery += " GROUP BY " + table + "." + attribute + ",site.id";
			
			if (i > 0) {
				query += "\nUNION\n"; 
			} else {
				schema = Utilities.getOutSchemaFromSql(subQuery);
			}
			
			query += subQuery;
		}
		query += "\n) as foo";
			
		return getResults(query, schema, s);
	}

	public static SliceStatistics collect(SliceKeyDefinition lhs, SliceKeyDefinition rhs) throws Exception {
	
		SliceStatistics lhsStats = collect(lhs);
		SliceStatistics rhsStats = collect(rhs);
		
		return SliceStatistics.intersectStatistics(lhsStats, rhsStats);
	}
		
	
	
	private static SliceStatistics getResults(String query, SecureRelRecordType schema, SliceKeyDefinition k) throws Exception {

		SliceStatistics histogram = new SliceStatistics(k);

		SqlQueryExecutor qe = new SqlQueryExecutor();
		QueryTable rawStats = qe.plainQuery(schema, query);
		

		
		// slice values, site_id, count
		List<Tuple> tuples = rawStats.tuples();
		for(Tuple t : tuples) {
			histogram.addDataSource(t);
		}
	
		return histogram;

	}
	
	


	
	
}
