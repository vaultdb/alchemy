package org.vaultdb.db.data;

import org.apache.commons.lang3.StringUtils;

import org.vaultdb.db.data.field.Field;
import org.vaultdb.db.data.field.FieldFactory;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.Comparator;
import java.util.List;
import java.util.Vector;

public class Tuple implements Comparator<Tuple>, Comparable<Tuple>, Serializable {

  List<Field> fields;
  transient SecureRelRecordType schema;

  public Tuple(SecureRelRecordType s, ResultSet values) throws Exception {
    ResultSetMetaData rsmd = values.getMetaData();

    schema = s;

    if (schema.getAttributes().size() != rsmd.getColumnCount()) {
      throw new Exception(
          "Result set column count does not match schema's "
              + rsmd.getColumnCount()
              + " != "
              + schema.getAttributes().size());
    }

    fields = new ArrayList<Field>();

    int i = 1;
    for (SecureRelDataTypeField attr : schema.getAttributes()) {
      Field f = FieldFactory.get(attr, values, i);
      fields.add(f);
      ++i;
    }

  }

  public Tuple(String boolStr, SecureRelRecordType schema) throws Exception {
    assert (schema.size() == boolStr.length());
    int offset = 0;
    fields = new ArrayList<Field>();

    for (SecureRelDataTypeField attr : schema.getAttributes()) {
      Field f = FieldFactory.get(attr);
      f.setValue(boolStr, offset);
      offset += f.size();
    }
  }

  public Tuple(BitSet bits, SecureRelRecordType schema) throws Exception {
    fields = new ArrayList<Field>();

    int start = 0;
    int end;

    for (SecureRelDataTypeField r : schema.getAttributes()) {
      Field f = FieldFactory.get(r);
      end = start + f.size();
      BitSet fieldBits = bits.get(start, end);
      // reverse field bits

      fieldBits = reverseBits(fieldBits, f.size());
      f.deserialize(fieldBits);
      fields.add(f);
      start = end;
    }
  }

  public static BitSet reverseBits(BitSet src, int length) {
    BitSet dst = new BitSet(length);

    for (int i = 0; i < length; ++i) dst.set(i, src.get(length - i - 1));

    return dst;
  }

  public SecureRelRecordType getSchema() {
    return schema;
  }

  public static List<Tuple> extractTuples(String bools, SecureRelRecordType schema)
      throws Exception {
    Tuple t = new Tuple(schema);
    int tSize = t.size();
    List<Tuple> output = new ArrayList<Tuple>();
    int offset = 0;

    assert (bools.length() % tSize == 0); // sizes align

    for (int i = 0; i < bools.length() / tSize; ++i) {
      String rawBits = bools.substring(offset, offset + tSize);
      output.add(new Tuple(rawBits, schema));
      offset += tSize;
    }

    return output;
  }

  public Tuple() {
    fields = new ArrayList<Field>();
    schema = new SecureRelRecordType();
  }

  // create blank tuple that matches schema
  public Tuple(SecureRelRecordType schema) throws Exception {
    fields = new ArrayList<Field>();

    for (SecureRelDataTypeField attr : schema.getAttributes()) {
      Field f = FieldFactory.get(attr);
      fields.add(f);
    }
  }

  public void addField(Field f) {
    fields.add(f);
    schema.addAttribute(f.getAttribute());
  }

  public int size() {
    int sum = 0;
    for (Field f : fields) {
      sum += f.size();
    }
    return sum;
  }

  public Field getField(int idx) {
    return fields.get(idx);
  }

  // Danger, no type checking
  // TODO: check that it is of the right type, e.g., schema says IntField, only accept IntField
  public void setField(int idx, Field f) {
    fields.set(idx, f);
  }

  public int getAttributeCount() {
    return fields.size();
  }

  public List<Boolean> serializeToBinary() {
    List<Boolean> output = new ArrayList<Boolean>();
    String source = this.toBinaryString();

    for (char c : source.toCharArray()) {
      Boolean v = (c == '0') ? false : true;
      output.add(v);
    }
    return output;
  }

  public BitSet toBitSet() {
    String src = this.toBinaryString();
    BitSet output = new BitSet(src.length());
    int writeIdx = 0;

    for (char c : src.toCharArray()) {
      output.set(writeIdx, (c == '0') ? false : true);
      ++writeIdx;
    }
    return output;
  }

  public String toBinaryString() {
    String ret = new String();

    for (Field f : fields) {
      ret += StringUtils.reverse(f.serializeToBinaryString());
    }

    return ret;
  }

  @Override
  public int hashCode() {
    int hash = 0;

    for (Field f : fields) {
      hash += f.hashCode();
    }

    return hash;
  }

  @Override
  public boolean equals(Object o) {
    if (!(o instanceof Tuple)) {
      return false;
    }

    Tuple t = (Tuple) o;

    if (t.fields.size() != this.fields.size()) {

      return false;
    }

    int i = 0;
    for (Field f : fields) {
      if (!f.equals(t.getField(i))) {

        return false;
      }
      ++i;
    }

    return true;
  }

  @Override
  public String toString() {
    return fields.toString();
  }

  public String printField(int field) {
    return fields.get(field).toString();
  }

  @Override
  public int compareTo(Tuple o) {

    assert (o.fields.size() == this.fields.size());

    for (int i = 0; i < fields.size(); ++i) {
      int cmp = fields.get(i).compareTo(o.getField(i));
      if (cmp != 0) {
        return cmp;
      }
    }
    return 0;
  }

  @Override
  public int compare(Tuple o1, Tuple o2) {
    return o1.compareTo(o2);
  }

  public List<Field> getFields() {
    return fields;
  }

  public List<SecureRelDataTypeField> getAttributes() {

    List<SecureRelDataTypeField> attrs = new ArrayList<SecureRelDataTypeField>();
    for (Field f : fields) {
      attrs.add(f.getAttribute());
    }
    return attrs;
  }

  public DBQueryProtos.Row toProto() throws Exception {
    DBQueryProtos.Row.Builder rowBuilder = DBQueryProtos.Row.newBuilder();
    int i = 0;
    for (Field f : fields) {
      rowBuilder.putColumn(i, f.toProto());
      i++;
    }
    return rowBuilder.build();
  }

  public byte[] getBytes() {
    int byteSize = 0;

    Vector<byte[]> bytes = new Vector<byte[]>();
    for (Field f : fields) {
      byte[] fieldBytes = f.getBytes();
      bytes.add(fieldBytes);
      byteSize += fieldBytes.length;
    }

    byte[] dst = new byte[byteSize];

    ByteBuffer target = ByteBuffer.wrap(dst);
    for (byte[] b : bytes) {
      target.put(b);
    }

    return target.array();
  }
}