package org.vaultdb.compiler.emp.generated;

import com.google.protobuf.InvalidProtocolBufferException;

import org.bytedeco.javacpp.BytePointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Const;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.Platform;
import org.vaultdb.compiler.emp.EmpProgram;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.util.Utilities;

@Platform(
    include = {"EmpJniJdbcDemo.h"},
    compiler = "cpp11")
@Namespace("EmpJniJdbcDemo")
public class EmpJniJdbcDemo extends EmpProgram {

  String query;

  public EmpJniJdbcDemo(int party, int port, String aQuery) {
    super(party, port);
    this.query = aQuery;
  }

  public static class EmpJniJdbcDemoClass extends Pointer {

    static {
      Loader.load();
    }

    public EmpJniJdbcDemoClass() {
      allocate();
    }

    private native void allocate();

    public native void addInput(String opName, @Const BytePointer bitString, int length);

    public native void run(int party, int port);

    // public native void setGeneratorHost(@StdString String host);

    public native BytePointer getOutput();

    public native int getOutputLength();

    public native void prepareOutput();
  }

  @Override
  public DBQueryProtos.Table runProgramGetProto() throws InvalidProtocolBufferException {
    EmpJniJdbcDemoClass theQuery = new EmpJniJdbcDemoClass();
    QueryTable input = null;

    if (generatorHost != null) {
      // theQuery.setGeneratorHost(generatorHost);
    }
    try {
      input = super.getInput(query);

      byte output[] = input.toProto().toByteArray();
      theQuery.addInput("base", new BytePointer(output), output.length);

    } catch (Exception e) {
      System.out.println("Failed to get input for " + query);
      e.printStackTrace();
      System.exit(-1);
    }

    theQuery.run(party, port);
    theQuery.prepareOutput();
    int size = theQuery.getOutputLength();
    byte[] outputBytes = new byte[size];
    BytePointer output = theQuery.getOutput();
    output.get(outputBytes, 0, size);
    DBQueryProtos.Table outputProtoTable = DBQueryProtos.Table.parseFrom(outputBytes);
    theQuery.close();
    return outputProtoTable;
  }

  // for testing
  public static void main(String[] args) throws Exception {

    int party = Integer.parseInt(args[0]);
    int port = Integer.parseInt(args[1]);

    String query = "SELECT patient_id FROM diagnoses";

    String setupFile = Utilities.getVaultDBRoot() + "/conf/setup.global";

    System.setProperty("vaultdb.setup", setupFile);
    String workerId = (party == 1) ? "alice" : "bob";

    System.setProperty("workerId", workerId);

    EmpJniJdbcDemo qc = new EmpJniJdbcDemo(party, port, query);
    DBQueryProtos.Table table = qc.runProgramGetProto();
    byte[] testing = table.toByteArray();
    System.err.write(testing);
  }
}
