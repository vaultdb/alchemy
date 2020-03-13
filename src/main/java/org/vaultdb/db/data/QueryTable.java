package org.vaultdb.db.data;

import org.apache.commons.lang3.StringUtils;

import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.plan.slice.SliceKeyDefinition;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelDataTypeField;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.FileUtilities;

import java.io.IOException;
import java.io.Serializable;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

public class QueryTable implements Serializable {

  // NB: tuples arrive with dummy tags, but the output schema does not include them
  private List<Tuple> tuples =
      null; // null until explicitly decrypted if init'd to PortableSecArray
  private transient SecureRelRecordType schema;
  private BitSet dummyTags = null;

  int tupleSize = 0;
  int tupleCount = 0;

  public QueryTable(List<Tuple> tuples) throws Exception {
    this.tuples = tuples;
    // this.dummytags = "0"; - Potentially add for consistency. This constructor should only
    // be used for expected output so dummies should not be necesarry.

    if (tuples != null && !tuples.isEmpty()) {
      schema = tuples.get(0).schema;
      tupleCount = tuples.size();
      tupleSize = schema.size();
    }
  }

  public QueryTable(BitSet bits, SecureRelRecordType s, int bitCount, boolean dummyTagged)
      throws Exception {
    schema = s;
    tupleSize = schema.size(); // without dummy tag
    int physicalTupleSize = tupleSize + 1;
    tupleCount = 0;

    Logger logger = SystemConfiguration.getInstance().getLogger();
    if (dummyTagged) {
      int dummyIdx = tupleSize;

      int inputTuples = bitCount / physicalTupleSize;

      logger.info(
          "\n\nHave "
              + inputTuples
              + " input tuples and "
              + bitCount
              + " input bits.  Tuple size = "
              + tupleSize
              + " bits.");

      assert (bitCount % physicalTupleSize == 0);

      tuples = new ArrayList<Tuple>();

      for (int i = 0; i < inputTuples; ++i) {
        boolean dummyTag = bits.get(i * physicalTupleSize + dummyIdx);
        // logger.info("Tuple " + i + " has dummy tag of " + dummyTag);
        if (!dummyTag) { // if dummyTag (at last idx) false
          BitSet tupleBits = bits.get(i * physicalTupleSize, (i + 1) * physicalTupleSize);

          Tuple t = new Tuple(tupleBits, schema);
          logger.info("Decoding a tuple " + t + "!");
          tuples.add(t);
          ++tupleCount;
        }
      }
    } else { // assume all values are real

      tupleCount = bitCount / tupleSize;

      assert (bitCount == (tupleCount * tupleSize));
      tuples = new ArrayList<Tuple>(tupleCount);

      for (int i = 0; i < tupleCount; ++i) {
        BitSet tupleBits = bits.get(i * tupleSize, (i + 1) * tupleSize);
        Tuple t = new Tuple(tupleBits, schema);
        tuples.add(t);
      }
    } // end no dummy tags case
  }

  public QueryTable(SecureRelRecordType outSchema) {
    schema = outSchema;
    tupleSize = schema.size();
    tupleCount = 0;
    tuples = new ArrayList<Tuple>();
  }

  public void addTuples(QueryTable src) {
    if (src == null) return;

    for (Tuple t : src.tuples) {
      tuples.add(t);
    }
  }

  // compare outputs by sorting them first
  public void sort() {
    Collections.sort(tuples);
  }

  public String printBitArray(boolean[] bits) {
    StringBuffer buf = new StringBuffer();
    for (boolean b : bits) {
      char o = b == true ? '1' : '0';
      buf.append(o);
    }
    return buf.toString();
  }

  public SecureRelRecordType getSchema() {
    return schema;
  }

  public void setSchema(SecureRelRecordType s) {
    schema = s;
  }

  // for use in testing
  public void truncateData(int limit) {
    if (tuples == null) return;

    while (tuples.size() > limit) {
      tuples.remove(tuples.size() - 1);
    }
  }

  public List<Tuple> tuples() {
    return tuples;
  }

  public void toBinaryFile(String dstFile) throws Exception {
    String tuplesStr = this.toBinaryString();
    FileUtilities.writeFile(dstFile, tuplesStr);
  }

  public String toBinaryString() throws Exception {
    String tupleStr = "";

    for (Tuple t : tuples) {
      tupleStr += t.toBinaryString();
    }
    return tupleStr;
  }

  public int tupleSize() throws IOException {
    if (schema != null) return schema.size();

    if (tuples == null || tuples.isEmpty()) {
      return 0;
    }

    return tuples.get(0).size();
  }

  public DBQueryProtos.Schema schemaToProto(SecureRelRecordType schema) throws Exception {
    DBQueryProtos.Schema.Builder schemaBuilder = DBQueryProtos.Schema.newBuilder();
    schemaBuilder.setNumColumns(schema.getFieldCount());
    for (int i = 0; i < schema.getFieldCount(); i++) {
      DBQueryProtos.ColumnInfo.Builder columnInfoBuilder = DBQueryProtos.ColumnInfo.newBuilder();
      columnInfoBuilder.setName(schema.getSecureField(i).getName());
      columnInfoBuilder.setColumnNumber(i);
      switch (schema.getSecureField(i).getType().getSqlTypeName()) {
        case INTEGER:
        case BIGINT:
          columnInfoBuilder.setType(DBQueryProtos.OIDType.BIGINT);
          break;
        case DECIMAL:
        case DOUBLE:
        case FLOAT:
          columnInfoBuilder.setType(DBQueryProtos.OIDType.DOUBLE);
          break;
        case TIMESTAMP_WITH_LOCAL_TIME_ZONE:
        case TIME_WITH_LOCAL_TIME_ZONE:
        case TIME:
        case DATE:
        case TIMESTAMP:
          columnInfoBuilder.setType(DBQueryProtos.OIDType.TIMESTAMP);
          break;
        case CHAR:
        case VARCHAR:
          columnInfoBuilder.setType(DBQueryProtos.OIDType.VARCHAR);
          break;
        case BOOLEAN:
        default:
          throw new Exception("Type not supported in protobufs");
      }
      schemaBuilder.putColumn(i, columnInfoBuilder.build());
    }
    return schemaBuilder.build();
  }

  public DBQueryProtos.Table toProto() throws Exception {
    if (tuples == null || tuples.isEmpty()) return null;

    return toProto(tuples);
  }

  public DBQueryProtos.Table toProto(List<Tuple> tuples) throws Exception {
    DBQueryProtos.Table.Builder tableBuilder = DBQueryProtos.Table.newBuilder();
    tableBuilder.setSchema(schemaToProto(schema));
    for (Tuple t : tuples) {
      tableBuilder.addRow(t.toProto());
    }
    return tableBuilder.build();
  }

  public boolean[] toBinary() throws Exception {
    if (tuples == null || tuples.isEmpty()) return null;

    return toBinary(tuples);
  }

  public static boolean[] toBinary(List<Tuple> tuples) throws Exception {
    if (tuples == null || tuples.isEmpty()) return null;
    int tCount = tuples.size();
    int tSize = tuples.get(0).schema.size();

    boolean[] tableBits = new boolean[tCount * tSize];
    int tIdx = 0;

    for (Tuple t : tuples) {

      List<Boolean> bitArray = t.serializeToBinary();
      if (bitArray.size() != tSize) {
        throw new Exception(
            "Tuple size mismatched! (probably owing to varchars not matching specified length)");
      }

      for (Boolean b : bitArray) {
        tableBits[tIdx] = b;
        ++tIdx;
      }
    }

    return tableBits;
  }

  public int tupleCount() {
    if (tuples == null) {
      return 0;
    }

    return tuples.size();
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }

    if (o instanceof QueryTable) {
      QueryTable qt = (QueryTable) o;

      if (qt.tuples.size() != tuples.size()) {
        return false;
      }

      if (qt.tuples == this.tuples) {
        return true;
      }

      if (qt.tuples.equals(this.tuples)) {
        return true;
      }
    }

    return false;
  }

  public Tuple getTuple(int i) {
    return tuples.get(i);
  }

  public String toString() {
    String ret = new String();
    List<String> attrNames = new ArrayList<String>();

    if (schema == null || schema.getAttributes().isEmpty()) {
      return null;
    }

    for (SecureRelDataTypeField r : schema.getAttributes()) {
      attrNames.add(r.getName());
    }
    ret = "(" + StringUtils.join(attrNames.toArray(), ", ") + ")\n";
    if (tuples != null)
      for (Tuple t : tuples) {
        ret += t.toString() + "\n";
      }
    return ret;
  }

  Tuple extractSliceValue(Tuple src, List<Integer> sliceIdxs) {
    Tuple ret = new Tuple();

    for (int i : sliceIdxs) {
      ret.addField(src.getField(i));
    }

    return ret;
  }

  public Map<Tuple, List<Tuple>> asSlices(SliceKeyDefinition key) throws Exception {
    // for each distinct value in slice key, list tuples associated with it
    Map<Tuple, List<Tuple>> slices = new LinkedHashMap<Tuple, List<Tuple>>();
    List<Integer> sliceIdxs = new ArrayList<Integer>();

    // what field(s) are associated with a slice?
    for (SecureRelDataTypeField slice : key.getAttributes()) {
      int i = 0;

      for (SecureRelDataTypeField r : schema.getAttributes()) {
        if (slice != null
            && r != null
            && slice.getName().equals(r.getName())
            && slice.getIndex() == r.getIndex()) {
          sliceIdxs.add(i);
          break;
        }
        ++i;
      }
    }

    if (sliceIdxs.isEmpty()) throw new Exception("No slices matched with source data!");

    for (Tuple t : tuples) {
      Tuple value = extractSliceValue(t, sliceIdxs);
      if (slices.containsKey(value)) {
        slices.get(value).add(t);
      } else {
        List<Tuple> tmp = new ArrayList<Tuple>();
        tmp.add(t);

        if (slices.containsKey(t))
          throw new Exception("Input not sorted by slice key!"); // no repeat keys

        slices.put(value, tmp);
      }
    }

    return slices;
  }

  public byte[] getTupleBytes() {
    if (tuples == null || tuples.isEmpty()) return null;

    int tupleLength = tuples.get(0).getBytes().length;
    byte dst[] = new byte[tupleLength * tuples.size()];
    ByteBuffer buffer = ByteBuffer.wrap(dst);

    for (Tuple t : tuples) {
      buffer.put(t.getBytes());
    }

    return buffer.array();
  }
}
