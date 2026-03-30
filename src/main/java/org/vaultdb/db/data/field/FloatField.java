package org.vaultdb.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;

import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;

import java.io.Serializable;
import java.util.BitSet;

public class FloatField extends Field<Float> implements Serializable {

  /** */
  private static final long serialVersionUID = -5006383705908340758L;
  int exponent;
  int fraction;

  public FloatField(SecureRelDataTypeField attr, Float v, SqlTypeName sqlType) {
    super(attr, sqlType);
    value = v;
    exponent = Float.floatToIntBits(v) & 0x7F800000;
    fraction = Float.floatToIntBits(v) & 0x807FFFFF;
  }

  public FloatField(SecureRelDataTypeField attr, SqlTypeName sqlType) {
    super(attr, sqlType);
    value = 0.0f;
    exponent = 0;
    fraction = 0;
  }

  @Override
  public String serializeToBinaryString() {
    String binString = Integer.toBinaryString(Float.floatToIntBits(value));
    // System.out.println(value);
    // System.out.println(binString);
    while (binString.length() < 32) {
      if (value < 0) {
        binString = "1" + binString;
      } else {
        binString = "0" + binString;
      }
    }
    assert (binString.length() == 32);
    return binString;
  }



  @Override
  public String toString() {
    return Float.toString(value);
  }

  @Override
  public int hashCode() {
    int hash = Float.hashCode(value);

    return hash;
  }

  public boolean equals(Object o) {
    if (o instanceof FloatField) {
      FloatField floatField = (FloatField) o;
      if (floatField.value.equals(this.value)) {
        return true;
      }
    }
    return false;
  }

  @Override
  public int childCompare(Field<Float> f) {
    if (f instanceof FloatField) {
      return this.value.compareTo(((FloatField) f).value);
    }
    return 0;
  }

  @Override
  public void setValue(String source, int sourceOffset) {
    String rawBits = source.substring(sourceOffset, sourceOffset + this.size());
    value = Float.intBitsToFloat(Integer.parseInt(rawBits));
  }

  @Override
  public void deserialize(BitSet src) {
    assert (src.size() == this.size());
    int temp = 0;
    for (int i = 0; i < src.size(); ++i) {
      boolean b = src.get(i);
      temp = (temp << 1) | (b ? 1 : 0);
    }
    value = Float.intBitsToFloat(temp);
  }

  @Override
  public byte[] getBytes() {
    int intBits = Float.floatToIntBits(value);
    return new byte[] {
      (byte) (intBits >> 24), (byte) (intBits >> 16), (byte) (intBits >> 8), (byte) (intBits)
    };
  }

  public DBQueryProtos.ColumnVal toProto() {
    DBQueryProtos.ColumnVal.Builder columnValBuilder = DBQueryProtos.ColumnVal.newBuilder();
    columnValBuilder.setDoubleField(value);
    // TODO(madhavsuresh): make this lineup with existing interface, this aligns with the OID fields
    // from postgres as opposed to anything in vaultdb-core
    columnValBuilder.setType(DBQueryProtos.OIDType.DOUBLE);
    return columnValBuilder.build();
  }
}
