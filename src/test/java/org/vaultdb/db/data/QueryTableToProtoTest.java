package org.vaultdb.db.data;

import org.vaultdb.BaseTest;
import org.vaultdb.TpcHBaseTest;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;

public class QueryTableToProtoTest extends TpcHBaseTest {

  public void testConvertQueryToProto() throws Exception {
    QueryTable t = SqlQueryExecutor.query("SELECT l_orderkey FROM lineitem LIMIT 10");
    t.toProto();
    System.out.println(t.toProto());
  }
}
