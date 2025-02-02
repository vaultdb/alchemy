package org.vaultdb.executor.plaintext;

import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.data.Tuple;
import org.vaultdb.executor.SegmentExecutor;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Logger;

// issue queries to one or more psql dbs
// automatically manages issuing query to all engines in network
// concatenating their results
public class SqlQueryExecutor {

  ConnectionManager connections = null;
  // say we want to collect statistics on diagnoses.patient_id
  // this will return a list of Tuples (pid, site, count)
  public SqlQueryExecutor() throws Exception {
    connections = ConnectionManager.getInstance();
  }

  public static QueryTable query(SecureRelRecordType outSchema, String query, Connection c)
      throws Exception {
    List<Tuple> queryOutput = new ArrayList<Tuple>();

    Statement st = c.createStatement();

    // TODO: remove hardcode, override how psql dialect is handled in calcite SQL generator
    query =
        query.replace(
            "TIMESTAMPDIFF(DAY, d2.timestamp_, d1.timestamp_)",
            "DATE_PART('day', d2.timestamp_ - d1.timestamp_)");

    ResultSet rs = st.executeQuery(query);
    if (!rs.isBeforeFirst()) // empty input set
    return new QueryTable(outSchema);

    while (rs.next()) {
      Tuple t = new Tuple(outSchema, rs);
      queryOutput.add(t);
    }

    rs.close();
    st.close();

    return new QueryTable(queryOutput);
  }

  // no joins between engines
  // just run each query once per data source and concatenate their collective outputs
  public QueryTable plainQuery(SecureRelRecordType outSchema, String query) throws Exception {

    SegmentExecutor executor = SegmentExecutor.getInstance();
    List<QueryTable> output = executor.runPlaintext(query, outSchema);

    if (output == null || output.isEmpty()) return null;

    Iterator<QueryTable> itr = output.iterator();
    QueryTable ret = itr.next();

    while (itr.hasNext()) {
      QueryTable t = itr.next();
      ret.addTuples(t);
    }

    return ret;
  }

  public static QueryTable query(String sql, String workerId) throws Exception {
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(sql);
    return SqlQueryExecutor.query( sql, outSchema, workerId);
  }

  public static QueryTable query(String sql) throws Exception {
    SecureRelRecordType outSchema = Utilities.getOutSchemaFromSql(sql);
    Connection unionConnection =
        ConnectionManager.getInstance().getConnection(ConnectionManager.getInstance().getUnioned());
    return SqlQueryExecutor.query(outSchema, sql, unionConnection);
  }

  public static QueryTable query(String sql, SecureRelRecordType outSchema, String workerId)
      throws Exception {
    Logger logger = SystemConfiguration.getInstance().getLogger();
    logger.fine("Starting query:\n" + sql);

    double start = System.nanoTime();
    Connection c = ConnectionManager.getInstance().getConnection(workerId);

    QueryTable result = query(outSchema, sql, c);
    double end = System.nanoTime();
    double elapsed = (end - start) / 1e9;
    logger.fine("Finished running in " + elapsed + " seconds.");

    return result;
  }

  // for CREATE TABLE, etc.
  public static void queryNoOutput(String sql, String workerId) throws SQLException, Exception {
    Connection c = ConnectionManager.getInstance().getConnection(workerId);

    Statement st = c.createStatement();
    st.execute(sql);
    st.close();
  }
}

// Security policy is viral, but only for the schema of what it touches
// thus SELECT publicAttribute, count(*) is only a count(*) against the schema (publicAttribute)
// if it were SELECT publicAttribute, count(*) FROM table WHERE privateAttribute = y,
// that's ok too because we never look at privateAttribute
// take care of this part in access control

// this is ok for stats collection because it is only viewed by the honest broker

// more broadly, user can see results derived from private data,
// but cannot see private data in plaintext
// just like honest broker
