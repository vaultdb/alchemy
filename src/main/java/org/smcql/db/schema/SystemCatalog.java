package org.smcql.db.schema;

import org.apache.calcite.util.Pair;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.db.data.field.IntField;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.config.WorkerConfiguration;
import org.smcql.executor.plaintext.SqlQueryExecutor;
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
public class SystemCatalog {

	
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
    
    
    Map<String, Pair<Long, Long> > tableCardinalities; // size of alice and bob's inputs
    
    
    static SystemCatalog instance;
    
    protected SystemCatalog(WorkerConfiguration config) throws ClassNotFoundException, SQLException {
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
		initializeTableCardinalities(dbConnection);
	}
	
    private void initializeTableCardinalities(Connection c) throws SQLException {
    	// list tables
    	String tableListQuery = "SELECT DISTINCT table_name FROM information_schema.tables WHERE table_schema='public'";
    	Statement st = c.createStatement();
    	
    	ResultSet rs = st.executeQuery(tableListQuery);

    	List<String> tables = new ArrayList<String>();
    	tableCardinalities = new HashMap<String, Pair<Long, Long>>();
    	
    	while (rs.next()) {
			String table = rs.getString(1);
			tables.add(table);		
    	}
    	
    	rs.close();
    	
    	for(String table : tables) {
			try {
				tableCardinalities.put(table, collectTableCardinalities(table));
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
    	}
    	
    		
	}
    
    
   
    private Pair<Long, Long> collectTableCardinalities(String table) throws Exception {
    	
    	String countQuery = "SELECT COUNT(*) FROM " + table;
    
    	String aliceId = ConnectionManager.getInstance().getAlice();
    	QueryTable alice = SqlQueryExecutor.query(countQuery, aliceId);
    	IntField countField = (IntField) alice.getTuple(0).getField(0);
    	long aliceCount = countField.value;
    	
    	String bobId = ConnectionManager.getInstance().getBob();
    	QueryTable bob = SqlQueryExecutor.query(countQuery, bobId);
    	countField = (IntField) bob.getTuple(0).getField(0);
    	long bobCount = countField.value;
    	
    	
    	Pair<Long, Long> counts = new Pair<Long, Long>(aliceCount, bobCount);
        	
    	return counts;
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


		while (rs.next()) {
			String table = rs.getString(1);
			String attr = rs.getString(2);
			partitionBy.put(table, attr);
		}
		
		rs.close();
	}

	
	public Pair<Long, Long> getTableCardinalities(String tableName) {
		return tableCardinalities.get(tableName);
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

	public static SystemCatalog getInstance() throws Exception {
    	if(instance == null) {
    		SystemConfiguration conf = SystemConfiguration.getInstance();
    		instance = new SystemCatalog(conf.getHonestBrokerConfig());
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
