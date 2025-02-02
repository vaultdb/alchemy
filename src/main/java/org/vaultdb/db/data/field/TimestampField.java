package org.vaultdb.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;

import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.sql.Timestamp;
import java.util.BitSet;

public class TimestampField extends Field<Timestamp> implements Serializable {

  /** */
  private static final long serialVersionUID = -6750183365124209223L;

  TimestampField(SecureRelDataTypeField attr, Timestamp timestamp, SqlTypeName sqlType) {
    super(attr, sqlType);
    value = timestamp;
    timestamp.getTime();
  }

  public TimestampField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
    super(attr, sqlType);
  }

  @Override
  public String serializeToBinaryString() {
    long epoch = value.getTime();
    String binString = Long.toBinaryString(epoch);

    while (binString.length() < this.size()) {
      binString = '0' + binString;
    }

    return binString;
  }

  public String toString() {
    return new String(Long.toString(value.getTime()));
  }

  @Override
  public int hashCode() {
    Long hash = value.getTime();
    return hash.hashCode();
  }

  public boolean equals(Object o) {
    if (o instanceof IntField) {
      TimestampField field = (TimestampField) o;
      if (field.value.getTime() == this.value.getTime()) {
        return true;
      }
    }
    return false;
  }

  @Override
  public int childCompare(Field f) {
    if (f instanceof TimestampField) {
      Long lhs = value.getTime();
      TimestampField rhsField = (TimestampField) f;
      
      Long rhs = rhsField.value.getTime();
      return lhs.compareTo(rhs);
    }
    return 0;
  }

  @Override
  public void setValue(String source, int sourceOffset) {
    String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
    Long epoch = Long.parseLong(rawBits, 2);
    value = new Timestamp(epoch);
  }

  @Override
  public void deserialize(BitSet src) {

    assert (src.size() == this.size());
    long epoch = 0;
    boolean b;

    for (int i = 0; i < src.size(); ++i) {
      b = src.get(i);
      epoch = (epoch << 1) | (b ? 1 : 0);
    }

    value = new Timestamp(epoch);
  }

  @Override
  public byte[] getBytes() {
    ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
    buffer.putLong(value.getTime()); // serialize to epoch
    return buffer.array();
  }

  public DBQueryProtos.ColumnVal toProto() {
    DBQueryProtos.ColumnVal.Builder columnValBuilder = DBQueryProtos.ColumnVal.newBuilder();
    columnValBuilder.setTimeStampField(value.getTime());
    // TODO(madhavsuresh): make this lineup with existing interface, this aligns with the OID fields
    // from postgres as opposed to anything in vaultdb-core
    columnValBuilder.setType(DBQueryProtos.OIDType.TIMESTAMP);
    return columnValBuilder.build();
  }
}
