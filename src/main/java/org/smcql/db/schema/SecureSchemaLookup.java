package org.smcql.db.schema;

import org.smcql.config.SystemConfiguration;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.type.SecureRelDataTypeField.SecurityPolicy;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

// decorator for calcite schema
// adds management of security policy for attributes
// also keeps track of how the tables are laid out - i.e., partitioning keys, replicated status
public class SecureSchemaLookup {

	
	// <table, <attrName, policy>
    Map<String, Map<String, SecurityPolicy>> accessPolicies;
    
    // <table, isReplicated>  // isReplicated is true if table is replicated over all data owners
    List<String> replicatedTables;
    
    // <table, attr> - what attribute is this table partitioned on?  
    // TODO: extend this for composite keys    
    Map<String, String> partitionBy;
    
    // primary keys
    // <table, list<attrs> >
    
    Map<String, List<String>> primaryKeys;
    
    
    
    
    static SecureSchemaLookup instance;
    
    protected SecureSchemaLookup(WorkerConfiguration config) throws ClassNotFoundException, SQLException {
		accessPolicies = new HashMap<String, Map<String, SecurityPolicy>>();
		String publicQuery = "SELECT table_name, column_name FROM information_schema.column_privileges WHERE grantee='public_attribute'";
		String protectedQuery = "SELECT table_name, column_name FROM information_schema.column_privileges WHERE grantee='protected_attribute'";
		String partitionByQuery = "SELECT table_name, column_name FROM information_schema.column_privileges WHERE grantee='partitioned_on'";
		String replicatedTablesQuery = "SELECT table_name FROM information_schema.table_privileges WHERE grantee='replicated'";
		String primaryKeysQuery = "SELECT pg_class.relname, pg_attribute.attname\n "
				+ "FROM pg_index, pg_class, pg_attribute, pg_namespace\n "
				+ "WHERE indrelid = pg_class.oid AND "
				+ "pg_namespace.nspname = 'public' AND "
				+ "pg_class.relnamespace = pg_namespace.oid AND "
				+  "pg_attribute.attrelid = pg_class.oid AND "
				+  "pg_attribute.attnum = any(pg_index.indkey) AND indisprimary";
		
		Connection dbConnection = config.getDbConnection();

		initializeSecurityPolicy(publicQuery, dbConnection, SecurityPolicy.Public);
		initializeSecurityPolicy(protectedQuery, dbConnection, SecurityPolicy.Protected);
		initializeReplicatedTables(replicatedTablesQuery, dbConnection);
		initializePartitioningKeys(partitionByQuery, dbConnection);
		initializePrimaryKeys(primaryKeysQuery, dbConnection);
	}
	
    private void initializePrimaryKeys(String sql, Connection c) throws SQLException {
    	Statement st = c.createStatement();
    	
    	ResultSet rs = st.executeQuery(sql);

    	primaryKeys = new HashMap<String, List<String>>();
    	
    	while (rs.next()) {
			String table = rs.getString(1);
			String attr = rs.getString(2);
			
			if(primaryKeys.containsKey(table)) {
				primaryKeys.get(table).add(attr);
			}
			else {
				List<String> keys = new ArrayList<String>();
				keys.add(attr);
				primaryKeys.put(table, keys);
			}
		
    	}
    	
    	
	}

	private void initializePartitioningKeys(String sql, Connection c) throws SQLException {
	
    	Statement st = c.createStatement();
		ResultSet rs = st.executeQuery(sql);
		partitionBy =  new HashMap<String, String>();

		System.out.println("***Have partition keys: ");

		while (rs.next()) {
			String table = rs.getString(1);
			String attr = rs.getString(2);
			partitionBy.put(table, attr);
			System.out.println("***" + table + ", " + attr);
		}
		
		rs.close();
	}

	private void initializeReplicatedTables(String sql, Connection c) throws SQLException {
    	Statement st = c.createStatement();
		ResultSet rs = st.executeQuery(sql);
		
		replicatedTables = new ArrayList<String>();
		
		while (rs.next()) {
			String table = rs.getString(1);
			replicatedTables.add(table);
		}
		rs.close();
	
	}

	public static SecureSchemaLookup getInstance() throws Exception {
    	if(instance == null) {
    		SystemConfiguration conf = SystemConfiguration.getInstance();
    		instance = new SecureSchemaLookup(conf.getHonestBrokerConfig());
    	}
    	return instance;
    }
	
	
	public static void resetInstance() {
		instance = null;
	}
	
	
	void initializeSecurityPolicy(String sql, Connection c, SecurityPolicy policy) throws SQLException {
		Statement st = c.createStatement();
		ResultSet rs = st.executeQuery(sql);
		
		while (rs.next()) {
			String table = rs.getString(1);
			String attr = rs.getString(2);
			if(accessPolicies.containsKey(table)) {
				accessPolicies.get(table).put(attr, policy);
			}
			else {
				Map<String, SecurityPolicy> tableEntries = new HashMap<String, SecurityPolicy>();
				tableEntries.put(attr, policy);
				accessPolicies.put(table, tableEntries);
			}
		}

		rs.close();

	}
	
	
	public Map<String, SecurityPolicy> tablePolicies(String tableName) {
		return accessPolicies.get(tableName);
	}
	
	public SecurityPolicy getPolicy(String table, String attr) {
		if(!accessPolicies.containsKey(table) || !accessPolicies.get(table).containsKey(attr))
			return SecurityPolicy.Private;
		return accessPolicies.get(table).get(attr);
	}
	
	public boolean isReplicated(String tableName) {
		return replicatedTables.contains(tableName);
	}
	
	public List<String> getPrimaryKey(String tableName) {
		return primaryKeys.get(tableName);
	}
	
	public String getPartitionKey(String tableName) {
		return partitionBy.get(tableName);
	}
	
}
