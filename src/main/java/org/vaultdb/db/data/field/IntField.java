package org.vaultdb.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;

import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.util.BitSet;

public class IntField extends Field<Long> implements Serializable {

  /** */
  private static final long serialVersionUID = -4887316771502090772L;


  public IntField(SecureRelDataTypeField attr, Long v, SqlTypeName sqlType) {
    super(attr, sqlType);
    value = v;
  }

  public IntField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
    super(attr, sqlType);
    value = 0L;
  }

  @Override
  public String serializeToBinaryString() {
    String binString = Long.toBinaryString(value);

    // pad it for length
    while (binString.length() < this.size()) {
      binString = '0' + binString;
    }

    return binString;
  }


  @Override
  public String toString() {
    return Long.toString(value);
  }

  @Override
  public int hashCode() {
    int hash = Long.hashCode(value);

    return hash;
  }

  public boolean equals(Object o) {
    if (o instanceof IntField) {
      IntField intField = (IntField) o;
      if (intField.value == this.value) {
        return true;
      }
    }
    return false;
  }

  @Override
  public int childCompare(Field<Long> f) {
    if (f instanceof IntField) {
      return value.compareTo(f.value);
    }
    return 0;
  }

  @Override
  public void setValue(String source, int sourceOffset) {
    String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
    value = Long.parseLong(rawBits, 2);
  }

  public void setValue(long newValue) {
    value = newValue;
  }

  public void addValue(long extraValue) {
    value += extraValue;
  }

  @Override
  public void deserialize(BitSet src) {
    value = 0L;
    boolean b;

    for (int i = 0; i < this.size(); ++i) {
      b = src.get(i);
      value = (value << 1) | (b ? 1 : 0);
    }
  }

  @Override
  public byte[] getBytes() {
    ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
    buffer.putLong(value);
    return buffer.array();
  }

  @Override
  public DBQueryProtos.ColumnVal toProto() {
    DBQueryProtos.ColumnVal.Builder columnValBuilder = DBQueryProtos.ColumnVal.newBuilder();
    columnValBuilder.setInt64Field(value);
    // TODO(madhavsuresh): make this lineup with existing interface, this aligns with the OID fields
    // from postgres as opposed to anything in vaultdb-core
    columnValBuilder.setType(DBQueryProtos.OIDType.BIGINT);
    return columnValBuilder.build();
  }
}
