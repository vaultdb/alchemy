#include <test/ompc/ompc_base_test.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include <operators/stored_table_scan.h>
//#include <operators/packed_table_scan.h>
//#include <data/secret_shared_data/secret_share_and_pack_data_from_query.h>
#include <boost/property_tree/json_parser.hpp>

#if __has_include("emp-rescu/emp-rescu.h")


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54334, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65428, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in unpacked buffer pool");

using namespace vaultdb;

class OMPCEmpTableTest : public OmpcBaseTest {
protected:
    void secretShareAndValidate(const std::string & sql, const SortDefinition & sort = SortDefinition());
};





// test encrypting a query table with a single int in EMP
TEST_F(OMPCEmpTableTest, ompc_secret_share_table_one_column) {

    std::string sql =  "SELECT l_orderkey FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";

    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(1);

    secretShareAndValidate(sql, collation);



}


// test secret a query table with a single string in EMP
TEST_F(OMPCEmpTableTest, ompc_secret_share_table_varchar) {

    string sql = "SELECT l_orderkey, l_linenumber, l_comment FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);

    secretShareAndValidate(sql, collation);

}



// test secret a query table with a single string in EMP
TEST_F(OMPCEmpTableTest, ompc_secret_share_customer) {

    string sql = "SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment FROM customer ORDER BY c_custkey";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);

    secretShareAndValidate(sql, collation);

}

// test more column types
TEST_F(OMPCEmpTableTest, ompc_secret_share_table) {

    std::string sql =  "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                       "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL
                       "FROM lineitem "
                       "WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                                                                               "ORDER BY l_orderkey, l_linenumber ";
    secretShareAndValidate(sql, DataUtilities::getDefaultSortDefinition(2));

}


TEST_F(OMPCEmpTableTest, ompc_secret_share_table_dummy_tag) {


    std::string sql = "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                      "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL
                      "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
                      "FROM lineitem "
                      " WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                                                                               "ORDER BY l_orderkey, l_linenumber ";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);
    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, true);

    input->order_by_ = collation;
    SecureTable *secret_shared = input->secretShare();
    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, true);
        expected->order_by_ = collation;
        DataUtilities::removeDummies(expected);
        ASSERT_EQ(*expected, *revealed);
    }

}


TEST_F(OMPCEmpTableTest, ompc_bit_packing_test) {


    std::string sql = "SELECT c_custkey, c_nationkey FROM customer WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1)";


    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, false);
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(1);
    input->order_by_ = collation;

    SecureTable *secret_shared = input->secretShare();
    //    cout << "Packed wire size: " << sizeof(emp::OMPCPackedWire) << " bytes, bit size: " << sizeof(emp::Bit) << " bytes, configured: " << sizeof(emp::Bit) << endl;
    // Expect: Packed wire size: 48 bytes, bit size: 192 bytes, configured: 48

    // c_custkey has 150 distinct vals, should have 8 bits
    ASSERT_EQ(8, secret_shared->getSchema().getField(0).size());
    // c_nationkey has 25 distinct vals, should have 5 bits
    ASSERT_EQ(5, secret_shared->getSchema().getField(1).size());

    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
    expected->order_by_ = collation;

    ASSERT_EQ(secret_shared->tuple_cnt_,  expected->tuple_cnt_);

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";
        delete revealed;
    }

    delete input;
    delete expected;
    delete secret_shared;

}

//TEST_F(OMPCEmpTableTest, ompc_test_packed_table_scan) {
//    std::string src_path = Utilities::getCurrentWorkingDirectory();
//    std::string packed_pages_path = src_path + "/packed_pages/";
//
//    std::string table_name = "customer";
//
//    PackedTableScan<Bit> packed_table_scan(FLAGS_unioned_db, table_name, packed_pages_path, FLAGS_party);
//    PackedColumnTable *packed_table = (PackedColumnTable *) packed_table_scan.run();
//
//    if(FLAGS_validation) {
//        std::string table_sql = "SELECT * FROM " + table_name + " ORDER BY c_custkey";
//
//        PlainTable *plain_input = DataUtilities::getQueryResults(db_name_, table_sql, false);
//        PackedColumnTable *expected = (PackedColumnTable *) plain_input->secretShare();
//
//        PlainTable *expected_plain = expected->revealInsecure(emp::PUBLIC);
//        PlainTable *observed_plain = packed_table->revealInsecure(emp::PUBLIC);
//
//        ASSERT_EQ(*expected_plain, *observed_plain);
//    }
//}

TEST_F(OMPCEmpTableTest, ompc_test_table_scan) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(Utilities::getCurrentWorkingDirectory() + "/table_config.json");
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    std::string table_name = "orders";
    int limit = -1;

    std::string table_sql;

    if(pt.count("tables") > 0) {
        boost::property_tree::ptree tables = pt.get_child("tables");

        for(ptree::const_iterator it = tables.begin(); it != tables.end(); ++it) {
            string current_table_name = "";

            if (it->second.count("table_name") > 0) {
                current_table_name = it->second.get_child("table_name").get_value<string>();
            } else {
                throw std::runtime_error("No table_name found in table_config.json");
            }

            if (current_table_name == table_name) {
                if(it->second.count("query") > 0) {
                    table_sql = it->second.get_child("query").get_value<string>();
                }
                else {
                    throw std::runtime_error("No query found in table_config.json");
                }
            }
        }
    }

    if(limit > 0) {
        table_sql += " LIMIT " + std::to_string(limit);
    }

    StoredTableScan<emp::Bit> *table_scan = new StoredTableScan<emp::Bit>(table_name, limit);
    auto observed = table_scan->run();

    if(FLAGS_validation) {
        PlainTable *plain_input = DataUtilities::getQueryResults(db_name_, table_sql, false);
        auto expected = plain_input->secretShare();
        expected->order_by_ = observed->order_by_;

        PlainTable *expected_plain = expected->revealInsecure(emp::PUBLIC);
        PlainTable *observed_plain = observed->revealInsecure(emp::PUBLIC);

        ASSERT_EQ(*expected_plain, *observed_plain);
    }
}

void OMPCEmpTableTest::secretShareAndValidate(const std::string & sql, const SortDefinition & sort) {

    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, false);

    input->order_by_ = sort;
    SecureTable *secret_shared = input->secretShare();

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort;

        ASSERT_EQ(*expected, *revealed);
        delete revealed;
        delete expected;
    }

    delete input;
    delete secret_shared;
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

//#else

//int main(int argc, char **argv) {
//   std::cout << "emp-rescu backend not found!" << std::endl;
//}

#endif
