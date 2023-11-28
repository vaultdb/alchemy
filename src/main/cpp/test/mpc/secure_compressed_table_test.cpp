#include "emp_base_test.h"
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include <test/support/tpch_queries.h>
#include "util/field_utilities.h"
#include "query_table/compression/compressed_table.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54325, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65454, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "compressed", "storage model for columns (column, wire_packed or compressed)");


class SecureCompressedTableTest :  public EmpBaseTest  { };

TEST_F(SecureCompressedTableTest, customer_nationname) {

    std::string customer_sql = "SELECT c_custkey, n_name "
                           "FROM customer JOIN nation ON c_nationkey = n_nationkey "
                           "ORDER BY c_custkey "
                           "LIMIT " + std::to_string(FLAGS_cutoff);
// will return empty set for db_name_ == empty_db_
auto base_table = DataUtilities::getQueryResults(db_name_, customer_sql, false);

auto expected_table = DataUtilities::getQueryResults(FLAGS_unioned_db, customer_sql, false);

    map<int, CompressionScheme> schemes = {
        {0,  CompressionScheme::PLAIN},
        {1,  CompressionScheme::DICTIONARY},
        {-1, CompressionScheme::PLAIN}};

    CompressedTable<bool> *plain_compressed_table =  new CompressedTable(base_table, schemes);

     if(FLAGS_party == manager_->sendingParty()) {

         // check that the dictionary size is correct
         string count_sql = "SELECT DISTINCT n_name FROM  (" + customer_sql + ") AS subquery";
         int cnt = DataUtilities::getTupleCount(db_name_, count_sql, false);

         auto dict = (DictionaryEncoding<bool> *) plain_compressed_table->column_encodings_.at(1);
         ASSERT_EQ(dict->dictionary_entry_cnt_, cnt);
         ASSERT_EQ(dict->dictionary_.size(), cnt);
         ASSERT_EQ(dict->reverse_dictionary_.size(), cnt);
         // should be 5 bits per entry, <= 25 entries
         ASSERT_EQ(plain_compressed_table->column_data_[1].size(), base_table->tuple_cnt_);
     }

     auto compressed_table = (CompressedTable<Bit> *) plain_compressed_table->secretShare();

    // reveals compressed data to both parties
    auto revealed_intermed = compressed_table->revealInsecure();

    auto revealed_decompressed = ((CompressedTable<bool> *) revealed_intermed)->decompress();
    ASSERT_EQ(*revealed_decompressed, *expected_table);

    // decompress it while remaining under MPC
    // not recommended, but verifying the functionality
    auto decompressed = compressed_table->decompress();
    auto revealed = decompressed->reveal();
    ASSERT_EQ(*base_table, *revealed);

    delete plain_compressed_table;
    delete base_table;
    delete decompressed;

}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

