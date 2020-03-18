package org.vaultdb.util;

import org.apache.calcite.util.Pair;
import org.apache.commons.io.FileUtils;

import org.bytedeco.javacpp.Loader;
import org.vaultdb.compiler.emp.EmpBuilder;
import org.vaultdb.compiler.emp.EmpRunnable;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.protos.DBQueryProtos;
import org.vaultdb.type.SecureRelRecordType;

import java.io.File;
import java.nio.file.DirectoryStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.logging.Logger;

public class EmpJniUtilities {

  public static String getPropertyFile() {
    String propertyFile =
        Utilities.getVaultDBRoot() + "/src/main/resources/org/bytedeco/javacpp/properties/";
    propertyFile += Loader.getPlatform();
    propertyFile += "-emp.properties";

    return propertyFile;
  }

  public static long convert(BitSet bits) {
    long value = 0L;
    for (int i = 0; i < bits.length(); ++i) {
      value += bits.get(i) ? (1L << i) : 0L;
    }
    return value;
  }

  public static List<Long> revealTableOutput(DBQueryProtos.Table alice, DBQueryProtos.Table bob)
      throws Exception {
    List<Long> output = new ArrayList<>();
    alice.getSchema().getNumColumns();
    assert (alice.getRowCount() == bob.getRowCount());
    for (int i = 0; i < alice.getRowCount(); i++) {
      for (Map.Entry<Integer, DBQueryProtos.ColumnInfo> k :
          alice.getSchema().getColumnMap().entrySet()) {
        assert (k.getValue().getType() == DBQueryProtos.OIDType.ENCRYPTED_INTEGER64);
        String aliceField = alice.getRow(i).getColumnMap().get(k.getKey()).getXorField();
        String bobField = bob.getRow(i).getColumnMap().get(k.getKey()).getXorField();
        Long aliceIntegerField = Long.parseLong(aliceField.trim());
        Long bobIntegerField = Long.parseLong(bobField.trim());
        output.add(bobIntegerField ^ aliceIntegerField);
      }
    }
    return output;
  }
  // output handling
  public static List<String> revealStringOutput(String alice, String bob, int tupleWidth) {
    assert (alice.length() == bob.length());

    BitSet aliceBits = EmpJniUtilities.stringToBitSet(alice);
    BitSet bobBits = EmpJniUtilities.stringToBitSet(bob);

    int tupleBits = tupleWidth * 8; // 8 bits / char
    int tupleCount = alice.length() / tupleBits;

    try {
      Logger logger = SystemConfiguration.getInstance().getLogger();
      logger.info("Decrypting " + tupleCount + " tuples.");

    } catch (Exception e) {
      e.printStackTrace();
    }

    BitSet decrypted = (BitSet) aliceBits.clone();
    decrypted.xor(bobBits);

    List<String> output = new ArrayList<String>();

    int readIdx = 0;
    for (int i = 0; i < tupleCount; ++i) {
      BitSet bits = decrypted.get(readIdx * 8, (readIdx + tupleWidth) * 8);
      String tuple = deserializeString(bits, tupleWidth);
      output.add(tuple);
      readIdx += tupleWidth;
    }

    return output;
  }

  public static BitSet stringToBitSet(String s) {
    BitSet b = new BitSet(s.length());

    for (int i = 0; i < s.length(); ++i) {
      b.set(i, (s.charAt(i) == '1') ? true : false);
    }

    return b;
  }

  public static String deserializeString(BitSet src, int stringLength) {
    assert (src.size() % 8 == 0);

    String value = new String();

    for (int i = 0; i < stringLength; ++i) {
      int n = 0;
      for (int j = 0; j < 8; ++j) {
        boolean b = src.get(i * 8 + j);
        n = (n << 1) | (b ? 1 : 0);
      }

      value += (char) n;
    }

    return value;
  }

  // class name includes package info
  // e.g., org.smcql.compiler.emp.generated.Count
  public static QueryTable runEmpLocal(String className, SecureRelRecordType outSchema)
      throws Exception {
    int empPort = getEmpPort();
    String fullyQualifiedClassName = getFullyQualifiedClassName(className);

    EmpRunnable aliceRunnable = new EmpRunnable(fullyQualifiedClassName, 1, empPort);
    EmpRunnable bobRunnable = new EmpRunnable(fullyQualifiedClassName, 2, empPort);

    EmpJniUtilities.buildEmpProgram(fullyQualifiedClassName);
 
    Thread alice = new Thread(aliceRunnable);
    alice.start();

    Thread bob = new Thread(bobRunnable);
    bob.start();

    alice.join();
    bob.join();

    // output consists of dummyTags followed by padded tuple payload
    String aliceOutput = aliceRunnable.getOutputString();
    String bobOutput = bobRunnable.getOutputString();

    // debugDecryption(aliceOutput, bobOutput, outSchema);

    BitSet decrypted = decrypt(aliceOutput, bobOutput);

    // out schema does not have a dummy tag in it
    // first bit of each tuple is a "hidden" dummy tag

    QueryTable decryptedTable = new QueryTable(decrypted, outSchema, aliceOutput.length(), true);

    return decryptedTable;
  }

  static void debugDecryption(String aliceOutput, String bobOutput, SecureRelRecordType outSchema) {
    int tupleWidthWithDummyTag = outSchema.size() + 1;
    int tupleCount = aliceOutput.length() / tupleWidthWithDummyTag;
    String decryptedString = new String();

    assert (aliceOutput.length() == bobOutput.length());

    for (int i = 0; i < aliceOutput.length(); ++i) {
      boolean aliceBit = (aliceOutput.charAt(i) == '0') ? false : true;
      boolean bobBit = (bobOutput.charAt(i) == '0') ? false : true;
      boolean outBit = aliceBit ^ bobBit;

      decryptedString += outBit ? '1' : '0';
    }

    for (int i = 0; i < tupleCount; ++i) {
      System.out.println(
          "Tuple "
              + i
              + ":"
              + decryptedString.substring(
                  i * tupleWidthWithDummyTag, (i + 1) * tupleWidthWithDummyTag));
    }
  }

  // for debugging this does a deep delete on previous builds

  public static void cleanEmpCode(String className) throws Exception {
    Path generatedFiles = Paths.get(Utilities.getCodeGenTarget());
    deleteFilesForPathByPrefix(generatedFiles, className);

    String osCode = Utilities.getCodeGenTarget() + "/" + Loader.getPlatform();
    FileUtils.deleteDirectory(new File(osCode));

    String cache = System.getProperty("user.home") + "/.javacpp/cache";
    FileUtils.deleteDirectory(new File(cache));

    String deleteGeneratedClassFiles =
        "rm "
            + Utilities.getVaultDBRoot()
            + "/target/classes/org/vaultdb/compiler/emp/generated/"
            + className
            + "*";
    Utilities.runCmd(deleteGeneratedClassFiles);
  }

  private static void deleteFilesForPathByPrefix(final Path path, final String prefix) {
    try (DirectoryStream<Path> newDirectoryStream = Files.newDirectoryStream(path, prefix + "*")) {
      for (final Path newDirectoryStreamItem : newDirectoryStream) {
        Files.delete(newDirectoryStreamItem);
      }
    } catch (final Exception e) {
    }
  }

  public static void createJniWrapper(
      String className,
      String dstFile,
      Map<String, String> inputs,
      Map<String, Pair<Long, Long>> cardinalities)
      throws Exception {
    // if it is a fully qualified class name, strip the prefix
    if (className.contains(".")) {
      className = className.substring(className.lastIndexOf('.' + 1));
    }

    Map<String, String> variables = new HashMap<String, String>();

    variables.put("queryName", className);
    variables.put("queryClass", className + "Class");

    if (inputs == null) {
      throw new Exception("Cannot run a query without input data!");
    }

    Iterator<Entry<String, String>> inputItr = inputs.entrySet().iterator();
    Iterator<Entry<String, Pair<Long, Long>>> cardinalityItr = cardinalities.entrySet().iterator();

    String inputSetup = new String();
    String cardinalitySetup = new String();

    // generate sql input statements
    while (inputItr.hasNext()) {
      Entry<String, String> pair = inputItr.next();
      String sql = (String) pair.getValue();
      sql = sql.replace('\n', ' ');
      String putStatement = "inputs.put(\"" + pair.getKey() + "Union\", \"" + sql + "\");\n";
      inputSetup += putStatement;
    }

    variables.put("sqlSetup", inputSetup);

    while (cardinalityItr.hasNext()) {
      Entry<String, Pair<Long, Long>> pair = cardinalityItr.next();
      Pair<Long, Long> value = pair.getValue();
      String variableName = pair.getKey() + "Cardinalities";
      String createStatement =
          "Pair<Long, Long> "
              + variableName
              + " = new Pair<Long, Long>("
              + value.left
              + "L, "
              + value.right
              + "L);\n";
      cardinalitySetup += createStatement;

      String putStatement =
          "obliviousCardinalities.put(\"" + pair.getKey() + "Union\", " + variableName + ");\n";
      cardinalitySetup += putStatement;
    }

    variables.put("cardinalitySetup", cardinalitySetup);

    String jniCode = CodeGenUtils.generateFromTemplate("/util/jni-wrapper.txt", variables);
    FileUtilities.writeFile(dstFile, jniCode);

    Logger logger = SystemConfiguration.getInstance().getLogger();
    logger.info("Writing jni wrapper to " + dstFile);
  }

  public static int getEmpPort() throws Exception {
    int port;
    // try local source
    String empPort = SystemConfiguration.getInstance().getProperty("emp-port");
    if (empPort != null && empPort != "") {
      port = Integer.parseInt(empPort); // TODO: check if it is numeric
    } else {
      // handle remote case
      port = Integer.parseInt(System.getProperty("emp.port"));
    }
    return port;
  }

  public static BitSet decrypt(BitSet alice, BitSet bob) {
    assert (alice.size() == bob.size());

    BitSet decrypted = (BitSet) alice.clone();
    decrypted.xor(bob);
    return decrypted;
  }

  public static BitSet decrypt(String alice, String bob) {
    assert (alice.length() == bob.length());

    BitSet aliceBits = stringToBitSet(alice);
    BitSet bobBits = stringToBitSet(bob);
    return decrypt(aliceBits, bobBits);
  }

  public static String getFullyQualifiedClassName(String className) throws Exception {
    String classPrefix = SystemConfiguration.getInstance().getProperty("generated-class-prefix");
    if (!className.startsWith(classPrefix)) {
      return classPrefix + "." + className;
    } else {
      return className;
    }
  }
  
	public static void buildEmpProgram(String className) throws Exception {
		// fork and exec EmpBuilder to make it see the new files
		

		String mvnLocation = SystemConfiguration.getInstance().getProperty("maven-location");
		
		String command =  mvnLocation +  " exec:java -Dexec.mainClass=\"org.vaultdb.compiler.emp.EmpBuilder\" -Dexec.args=\"" + className + "\"";
		System.out.println("Running command: " + command);
		Utilities.runCmd(command);
		
		

	}
	
}
