import com.dbquery.DBQueryProtos;
import com.dbquery.DBQueryProtos.OIDType;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Types;

public class DBQuery {

  //"jdbc:postgresql://localhost/vaultdb_",
  private String sqlString, dbString, user, password;
  private Connection conn;
  private ResultSet rs;
  private ResultSetMetaData rsmd;

  public DBQuery(String sqlString, String dbString, String user, String password) {
    this.sqlString = sqlString;
    this.dbString = dbString;
    this.user = user;
    this.password = password;
  }

  public Connection connect() {
    try {
      this.conn =
          DriverManager.getConnection(
              dbString,
              user,
              password);
    } catch (SQLException e) {
      throw new IllegalStateException("Unexpected value: " + e.getMessage());
    }
    return this.conn;
  }

  public void executeQuery() {
    this.connect();
    try {
      Statement stmt = conn.createStatement();
      this.rs = stmt.executeQuery(sqlString);
    } catch (SQLException e) {
      throw new IllegalStateException("Unexpected value: " + e.getMessage());
    }
  }

  private OIDType getType(int jdbcType) {
    switch (jdbcType) {
    case Types.INTEGER:
      return OIDType.INTEGER;
    case Types.FLOAT:
    case Types.DOUBLE:
      return OIDType.DOUBLE;
    case Types.VARCHAR:
    case Types.CHAR:
      return OIDType.VARCHAR;
    case Types.NUMERIC:
      return OIDType.NUMERIC;
    case Types.TIMESTAMP:
      return OIDType.TIMESTAMP;
    case Types.TIME:
      return OIDType.TIME;
    case Types.BIGINT:
      return OIDType.BIGINT;
    default:
      return OIDType.UNSUPPORTED;
    }
  }

  public DBQueryProtos.Table runQuery() {
    this.executeQuery();
    return getTable(this.getSchema());
  }

  public DBQueryProtos.Table getTable(DBQueryProtos.Schema schema) {
    DBQueryProtos.Table.Builder tableBuilder = DBQueryProtos.Table.newBuilder();
    try {
      while (rs.next()) {
        DBQueryProtos.Row.Builder rowBuilder = DBQueryProtos.Row.newBuilder();
        for (int i = 1; i <= schema.getNumColumns(); i++) {
          DBQueryProtos.ColumnVal.Builder columnValBuilder = DBQueryProtos.ColumnVal.newBuilder();
          switch (schema.getColumnMap().get(i).getType().getNumber()) {
          case OIDType.INTEGER_VALUE:
          case OIDType.BIGINT_VALUE:
            columnValBuilder.setInt64Field(rs.getInt(i));

            break;
          case OIDType.NUMERIC_VALUE:
          case OIDType.DOUBLE_VALUE:
            //TODO(madhavsuresh): watch out, overloaded numeric with double.
            columnValBuilder.setDoubleField(rs.getDouble(i));
            break;
          case OIDType.TIME_VALUE:
            columnValBuilder.setTimeStampField(rs.getTimestamp(i).getTime());
            break;
          case OIDType.VARCHAR_VALUE:
            columnValBuilder.setStrField(rs.getString(i));
            break;
          default:
            throw new IllegalStateException("Unexpected value: " + schema.getColumnMap().get(i).getType().getNumber());
          }
          columnValBuilder.setTypeValue(schema.getColumnMap().get(i).getType().getNumber());
          rowBuilder.putColumn(i - 1, columnValBuilder.build());
        }
        tableBuilder.addRow(rowBuilder.build());
      }
    } catch (SQLException e) {
      System.out.println(e);
      throw new IllegalStateException("Unexpected value: " + e.getMessage());
    }
    return tableBuilder.build();
  }

  public DBQueryProtos.Schema getSchema() {
    DBQueryProtos.Schema.Builder schemaBuilder = DBQueryProtos.Schema.newBuilder();
    try {
      ResultSetMetaData rsmd = this.rs.getMetaData();
      schemaBuilder.setNumColumns(rsmd.getColumnCount());
      for (int i = 1; i <= rsmd.getColumnCount(); i++) {
        DBQueryProtos.ColumnInfo col = DBQueryProtos.ColumnInfo.newBuilder()
            .setName(rsmd.getColumnName(i))
            .setTableName(rsmd.getTableName(i))
            .setType(getType(rsmd.getColumnType(i)))
            .setColumnNumber(i)
            .build();
        //Vaultdb is zero indexed
        schemaBuilder.putColumn(i - 1, col);
      }
    } catch (SQLException e) {
      throw new IllegalStateException("Unexpected value: " + e.getMessage());
    }
    return schemaBuilder.build();
  }
}
