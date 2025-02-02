package org.vaultdb.db.data.field;

import org.apache.calcite.sql.type.SqlTypeName;

import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;

import java.io.IOException;
import java.io.Serializable;
import java.util.BitSet;

public class StringField extends Field<String> implements Serializable {
  /** */
  private static final long serialVersionUID = 7638908984744040905L;


  public StringField(SecureRelDataTypeField attr, SqlTypeName sqlType) throws IOException {
    super(attr, sqlType);
    value = new String();
    size = attr.size();
  }

  public StringField(SecureRelDataTypeField attr, String value, SqlTypeName sqlType)
      throws Exception {
    super(attr, sqlType);
    this.value = value;
    if (value != null && value.length() > (attr.size() / 8)) {
      throw new Exception(
          "String exceeds specified size!"
              + value
              + " at "
              + value.length()
              + " for attribute "
              + attr);
    }
    size = attr.size();
  }

  @Override
  public String serializeToBinaryString() {
    String ret = new String();

    if (value != null) {
      for (char c : value.toCharArray()) {
        ret += serializeChar(c);
      }
    }

    while (ret.length() < size()) {
      ret += '0';
    }

    return ret;
  }

  private String serializeChar(char c) {

    String ret = Integer.toBinaryString((int) c);
    while (ret.length() < 8) {
      ret = "0" + ret;
    }

    return ret;
  }

  public String toString() {
    return new String(value);
  }

  @Override
  public int hashCode() {
    return value.hashCode();
  }

  public boolean equals(Object o) {
    if (o instanceof StringField) {
      StringField charField = (StringField) o;
      String localValue = this.value;
      String localOther = charField.value;

      while (localOther.length() < localValue.length()) { // fix null padding
        localOther += "\0";
      }

      while (localOther.length() > localValue.length()) { // fix null padding
        localValue += "\0";
      }
      if (localOther.equals(localValue)) {
        return true;
      }
    }
    return false;
  }

  @Override
  public int childCompare(Field<String> f) {
    if (f instanceof StringField) {
      return value.compareTo(((StringField) f).value);
    }
    return 0;
  }

  @Override
  public void setValue(String source, int startOffset) {
    value = new String();

    for (int i = 0; i < size / 8; ++i) {
      value += getCharacter(source, startOffset + i * 8);
    }
  }

  public void setValue(String newValue) {
    value = newValue;
  }

  private char getCharacter(String source, int startOffset) {
    String rawBits = source.substring(startOffset, startOffset + 8);
    while (rawBits.length() < this.size()) {
      rawBits += "0";
    }

    int value = Integer.parseInt(rawBits, 2);

    return (char) value;
  }

  @Override
  public void deserialize(BitSet src) {
    // assert(src.size() == this.size);

    value = new String();
    int chars = this.size / 8;

    for (int i = 0; i < chars; ++i) {
      BitSet bits = src.get(i * 8, (i + 1) * 8);
      value += deserializeChar(bits);
    }
  }

  private char deserializeChar(BitSet bits) {

    int n = 0;
    for (int i = 0; i < 8; ++i) {
      boolean b = bits.get(i);
      n = (n << 1) | (b ? 1 : 0);
    }

    
    return (char) n;
  }

  @Override
  public byte[] getBytes() {
    return value.getBytes();
  }

  public DBQueryProtos.ColumnVal toProto() {
    DBQueryProtos.ColumnVal.Builder columnValBuilder = DBQueryProtos.ColumnVal.newBuilder();
    columnValBuilder.setStrField(value);
    // TODO(madhavsuresh): make this lineup with existing interface, this aligns with the OID fields
    // from postgres as opposed to anything in vaultdb-core
    columnValBuilder.setType(DBQueryProtos.OIDType.VARCHAR);
    return columnValBuilder.build();
  }
}
