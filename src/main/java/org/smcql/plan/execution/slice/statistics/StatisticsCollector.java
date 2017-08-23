package org.smcql.plan.execution.slice.statistics;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.calcite.rel.logical.LogicalFilter;
import org.smcql.codegen.smc.operator.support.RexNodeUtilities;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.Tuple;
import org.smcql.db.data.field.IntField;
import org.smcql.type.SecureRelDataTypeField;
import org.smcql.type.SecureRelRecordType;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.plan.slice.SliceKeyDefinition;
import org.smcql.util.Utilities;

public class StatisticsCollector {
	
	// performed once per secure leaf
	public static SliceStatistics collect(SliceKeyDefinition s) throws Exception {
		//assert(s.getAttributes().size() == 1);			
		SecureRelDataTypeField keyField = s.getAttributes().get(0);
		
		//String table = keyField.getStoredTable();
		String attribute = keyField.getStoredAttribute();
		
		
		
		//assert(keyField.getFilters().size() == 1); 
		List<String> predicates = s.getFilters();
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
				schema = Utilities.getOutSchemaFromString(subQuery);
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
