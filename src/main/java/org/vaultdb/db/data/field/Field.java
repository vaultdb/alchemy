package org.vaultdb.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;

import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.TypeMap;

import java.io.Serializable;
import java.util.BitSet;
import java.util.Comparator;

// individual attribute value inside a tuple
// for use in slice values and for passing around data between SMC and plaintext
// hashcode() and equals() are compare by value, not source attribute name / type

public class Field implements Comparator<Field>, Comparable<Field>, Serializable {

  transient SecureRelDataTypeField srcAttribute = null;
  int size = 0;

  // psql ResultSet r, int rsetIdx
  Field(SecureRelDataTypeField attr, SqlTypeName sqlType) {
    srcAttribute = attr;
    size = TypeMap.getInstance().sizeof(sqlType);
  }

  // bytes
  public int size() {
    return size;
  }

  public String serializeToBinaryString() {
    return null;
  }

  // meant to be overriden by children
  public int childCompare(Field f) {
    return 0;
  }

  // implemented in child classes
  @Override
  public int compareTo(Field o) {
    assert this.getClass().equals(o.getClass());
    return this.childCompare(o);
  }

  @Override
  public int compare(Field o1, Field o2) {
    assert (o1.getClass().equals(o2.getClass()));
    return o1.compareTo(o2);
  }

  public SecureRelDataTypeField getAttribute() {
    return srcAttribute;
  }

  // for override
  // source is a string of bools
  public void setValue(String source, int startOffset) {}

  // for override
  public byte[] getBytes() {
    return null;
  }

  public DBQueryProtos.ColumnVal toProto() throws Exception {
    throw new Exception("Base class not implemented");
  }

  public void deserialize(BitSet src) {}
}
