package org.vaultdb.util;

import org.vaultdb.protos.DBQueryProtos;
/* This class is here to make sure that proto building works on all platforms. If this file doesnt compile, things
are broken
If this class fails to build, please contact @madhavsuresh.
 */
public class ProtoCanary {
  private static int test() {
    DBQueryProtos.Schema schema = DBQueryProtos.Schema.newBuilder().setNumColumns(10).build();
    return 0;
  }
}
