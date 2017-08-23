package org.smcql.db.schema;

import java.io.Serializable;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;
import java.util.logging.Logger;

import org.smcql.executor.config.WorkerConfiguration;

// create a list of JSQLParser tables from a schema file
// use this for type management in smc code
public class StoredDatabase implements Serializable {

	// list of tables and their column definitions
	private LinkedHashMap<String, StoredTable> tables;
	private WorkerConfiguration worker;
	String name;
	Logger logger;

	
	protected StoredDatabase() {  }
	
	
	// ddl consists of a set of create table statements
	protected StoredDatabase(WorkerConfiguration config) throws Exception {

		Connection dbConnection = config.getDbConnection();
		//http://www.java2s.com/Code/Java/Database-SQL-JDBC/GetTablePrivileges.htm

		ResultSet privileges = null;
	    DatabaseMetaData meta = dbConnection.getMetaData();
	    // The '_' character represents any single character.
	    // The '%' character represents any sequence of zero
	    // or more characters.
	    privileges = meta.getColumnPrivileges(dbConnection.getCatalog(), "%", "%", "%");
	   //privileges = meta.getTablePrivileges(dbConnection.getCatalog(), "%", "diagnoses");
		System.out.println("Looking at privileges" + privileges);

	    while (privileges.next()) {
	    	System.out.println("Iterating over priv");
	      String catalog = privileges.getString("TABLE_CAT");
	      String schema = privileges.getString("TABLE_SCHEM");
	      String tableName = privileges.getString("TABLE_NAME");
	      String privilege = privileges.getString("PRIVILEGE");
	      String grantor = privileges.getString("GRANTOR");
	      String grantee = privileges.getString("GRANTEE");
	      String isGrantable = privileges.getString("IS_GRANTABLE");

	      System.out.println("table name:" + tableName);
	      System.out.println("catalog:"+catalog);
	      System.out.println("schema:"+ schema);
	      System.out.println("privilege:"+privilege);
	      System.out.println("grantor:"+grantor);
	      System.out.println("isGrantable:"+isGrantable);
	      System.out.println("grantee:"+grantee);
	    }
	}
	
	public String getName() {
		return name;
	}
	
	public StoredTable getTable(String tableName) {
		return tables.get(tableName);
	}
		
	public int getTableCount() {
		return tables.size();
	}
	
	
	public Iterator<Entry<String, StoredTable>> getTableIterator() {
		return  tables.entrySet().iterator();
		
	}
	
	public WorkerConfiguration getConnection() {
		return worker;
	}
	

}
