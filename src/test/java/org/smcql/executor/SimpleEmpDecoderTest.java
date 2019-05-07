package org.smcql.executor;

import org.smcql.codegen.QueryCompiler;
import org.smcql.codegen.sql.SqlGenerator;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.parser.tpch.TpcHBaseTest;
import org.smcql.plan.SecureRelRoot;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.EmpJniUtilities;
import org.smcql.util.FileUtils;
import org.smcql.util.Utilities;

import java.util.Arrays;


public class SimpleEmpDecoderTest extends TpcHBaseTest {
    protected void setUp() throws Exception {

        super.setUp();

    }


    public void testQuery01() throws Exception {
        String sql = QUERIES.get(0);
        String testName = "q" + String.valueOf(1);
        runQuery(testName, sql);
    }

    public void testQuery02() throws Exception {
        String sql = QUERIES.get(1);
        String testName = "q" + String.valueOf(2);
        runQuery(testName, sql);
    }

    public void testQuery03() throws Exception {
        String sql = QUERIES.get(2);
        String testName = "q" + String.valueOf(3);
        runQuery(testName, sql);
    }

    public void testQuery04() throws Exception {
        String sql = QUERIES.get(3);
        String testName = "q" + String.valueOf(4);
        runQuery(testName, sql);
    }

    public void testQuery05() throws Exception {
        String sql = QUERIES.get(4);
        String testName = "q" + String.valueOf(5);
        runQuery(testName, sql);
    }

    // want this to be a split aggregate
    // each DO produces a partial sum
    public void testQuery06() throws Exception {
        String sql = QUERIES.get(5);
        String testName = "q" + String.valueOf(6);
        runQuery(testName, sql);
    }

    public void testQuery07() throws Exception {
        String sql = QUERIES.get(6);
        String testName = "q" + String.valueOf(7);
        runQuery(testName, sql);
    }

    public void testQuery08() throws Exception {
        String sql = QUERIES.get(7);
        String testName = "q" + String.valueOf(8);
        runQuery(testName, sql);
    }

    public void testQuery09() throws Exception {
        String sql = QUERIES.get(8);
        String testName = "q" + String.valueOf(9);
        runQuery(testName, sql);
    }

    public void testQuery10() throws Exception {
        String sql = QUERIES.get(9);
        String testName = "q" + String.valueOf(10);
        runQuery(testName, sql);
    }

    public void testQuery11() throws Exception {
        String sql = QUERIES.get(10);
        String testName = "q" + String.valueOf(11);
        runQuery(testName, sql);
    }

    public void testQuery12() throws Exception {
        String sql = QUERIES.get(11);
        String testName = "q" + String.valueOf(12);
        runQuery(testName, sql);
    }

    public void testQuery13() throws Exception {
        String sql = QUERIES.get(12);
        String testName = "q" + String.valueOf(13);
        runQuery(testName, sql);
    }

    public void testQuery14() throws Exception {
        String sql = QUERIES.get(13);
        String testName = "q" + String.valueOf(14);
        runQuery(testName, sql);
    }

    public void testQuery15() throws Exception {
        String sql = QUERIES.get(14);
        String testName = "q" + String.valueOf(15);
        runQuery(testName, sql);
    }

    public void testQuery16() throws Exception {
        String sql = QUERIES.get(15);
        String testName = "q" + String.valueOf(16);
        runQuery(testName, sql);
    }

    public void testQuery17() throws Exception {
        String sql = QUERIES.get(16);
        String testName = "q" + String.valueOf(17);
        runQuery(testName, sql);
    }

    public void testQuery18() throws Exception {
        String sql = QUERIES.get(17);
        String testName = "q" + String.valueOf(18);
        runQuery(testName, sql);
    }

    public void testQuery19() throws Exception {
        String sql = QUERIES.get(18);
        String testName = "q" + String.valueOf(19);
        runQuery(testName, sql);
    }

    public void testQuery20() throws Exception {
        String sql = QUERIES.get(19);
        String testName = "q" + String.valueOf(20);
        runQuery(testName, sql);
    }

    public void testQuery21() throws Exception {
        String sql = QUERIES.get(20);
        String testName = "q" + String.valueOf(21);
        runQuery(testName, sql);
    }

    public void testQuery22() throws Exception {
        String sql = QUERIES.get(21);
        String testName = "q" + String.valueOf(22);
        runQuery(testName, sql);
    }



    protected void runQuery(String testName, String sql) throws Exception {


        // Make Querycompiler using a simple string matching the input mask
        // This can be changed to take input sql in the future

        String query = "SELECT c_nationkey FROM customer";
        // Currently hard to decouple testing this from a more complex query

        SecureRelRoot secRoot = new SecureRelRoot(testName, query);
        QueryCompiler qc = new QueryCompiler(secRoot);

        // Make known boolean input - need to make smarter way to autogenerate this from more advanced SQL queries
        // if want to extend this type of testing
        int[] mylist = new int[]{1,3,4,8,13,15,17,18,20};

        // Randomly create boolean mask for each of the above integers
        // Currently just hardcoded
        String bitmask = "111110000";

        // bits are the input bits
        String bits = "";
        bits += bitmask;

        String target = "";

        // For loop to create full input bits and target
        for (int i = 0; i < mylist.length; ++i) {
            char search = '1';

            bits += String.format("%64s", Integer.toBinaryString(mylist[i])).replace(' ', '0');

            if (bitmask.charAt(i) == search){
                target += String.format("%64s", Integer.toBinaryString(mylist[i])).replace(' ', '0');
            }
        }


        // Create Decryped bool
        boolean[] decrypted = new boolean[bits.length()];

        // Logic to input Mask portion of decrypted boolean
        for (int i = 0; i < bitmask.length(); ++i){
            char search = '1';

            if (bitmask.charAt(i) == search){
                decrypted[i] = false;
            }
            else{
                decrypted[i] = true;
            }
        }

        // Logic for Tuple representation
        for (int i = bitmask.length(); i < bits.length(); ++i){
            char search = '1';

            if (bits.charAt(i) == search){
                decrypted[i] = true;
            }
            else{
                decrypted[i] = false;
            }
        }


        // ------------------------------------------------------------------------------------------
        // Perform current logic implemented to filter out dummy tuples using the encrypted values

        int tupleLen = qc.getOutSchema().size() + 1;  // +1 for the dummy tag
        int tupleCount = decrypted.length / tupleLen;

        // TODO: Keith figure out how to do this with pointer arithmetic
        // This can be done by impleming a smarter XOR process where we only selectively
        // decrypt non-dummy tuples

        boolean[] dummyTags = Arrays.copyOfRange(decrypted, 0, tupleCount);
        boolean[] tuples = Arrays.copyOfRange(decrypted, tupleCount, decrypted.length);

        QueryTable testTable =  new QueryTable(dummyTags, tuples, qc.getOutSchema());

        // Write filtered querytable to bit string
        String testBool = testTable.toBinaryString();

        // Compare known values to known
        assertEquals(testBool, target);

    }


}