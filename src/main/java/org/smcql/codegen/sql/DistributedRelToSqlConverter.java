package org.smcql.codegen.sql;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.calcite.rel.core.TableScan;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlIdentifier;
import org.apache.calcite.sql.parser.SqlParserPos;


// transform a SQL statement into one that runs on both alice and bob by unioning their inputs
public class DistributedRelToSqlConverter extends   ExtendedRelToSqlConverter{

	public DistributedRelToSqlConverter(SqlDialect dialect) {
		super(dialect);
	}

	@Override
	public Result visit(TableScan e) {
		String tableName = e.getTable().getQualifiedName().get(0);
		String unionedName = " ((SELECT * FROM " + tableName + ") UNION ALL (SELECT * FROM remote_" + tableName + "))"  + " all_" + tableName;
		List<String> name = new ArrayList<String>();
		name.add(unionedName);
		
	    final SqlIdentifier identifier =
	            new SqlIdentifier(name, SqlParserPos.ZERO);

	    return result(identifier, Collections.singletonList(Clause.FROM), e);

	}
	
	
}
