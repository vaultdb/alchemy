#include "plain_base_test.h"
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include <test/support/tpch_queries.h>
#include <operators/sql_input.h>
#include <operators/sort_merge_join.h>
#include "util/field_utilities.h"
#include "query_table/compression/compressed_table.h"
#include "query_table/compression/plain_dictionary_encoding.h"

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "compressed", "storage model for columns (column or compressed)");

class CompressedTableTest :  public PlainBaseTest  {

};

// base case: no compression
// derived from EmpTableTest
TEST_F(CompressedTableTest, uncompressed) {
    std::string sql =  "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                       "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL
                       "FROM lineitem "
                       "WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                                                                               "ORDER BY l_orderkey, l_linenumber ";

    auto base_table = DataUtilities::getQueryResults(db_name_, sql, false);

    map<int, CompressionScheme> schemes = {
            {0, CompressionScheme::PLAIN},
            {1, CompressionScheme::PLAIN},
            {2, CompressionScheme::PLAIN},
            {3, CompressionScheme::PLAIN},
            {4, CompressionScheme::PLAIN},
            {-1, CompressionScheme::PLAIN}};

    auto compressed_table = new CompressedTable(base_table, schemes);

    auto decompressed = compressed_table->decompress();
    ASSERT_EQ(*base_table, *decompressed);
    delete compressed_table;
    delete base_table;
    delete decompressed;

}

// TODO: add tests for add'l compression schemes
TEST_F(CompressedTableTest, customer_nationname) {

    std::string customer_sql = "SELECT c_custkey, n_name "
                           "FROM customer JOIN nation ON c_nationkey = n_nationkey "
                           "ORDER BY c_custkey "
                           "LIMIT " + std::to_string(FLAGS_cutoff);

 auto base_table = DataUtilities::getQueryResults(db_name_, customer_sql, false);

map<int, CompressionScheme> schemes = {
        {0, CompressionScheme::PLAIN},
        {1, CompressionScheme::DICTIONARY},
        {-1, CompressionScheme::PLAIN}};

    auto compressed_table = new CompressedTable(base_table, schemes);

    // check that the dictionary size is correct
    string count_sql = "SELECT DISTINCT n_name FROM  (" + customer_sql + ") AS subquery";
    int cnt = DataUtilities::getTupleCount(db_name_, count_sql, false);

    auto dict = (DictionaryEncoding<bool> *) compressed_table->column_encodings_.at(1);
    ASSERT_EQ(dict->dictionary_entry_cnt_, cnt);
    ASSERT_EQ(dict->dictionary_.size(), cnt);
    ASSERT_EQ(dict->reverse_dictionary_.size(), cnt);
    // should be 1 byte per entry, <= 25 entries
    ASSERT_EQ(compressed_table->column_data_[1].size(), base_table->tuple_cnt_);

    // decompress it
    auto decompressed = compressed_table->decompress();
    ASSERT_EQ(*base_table, *decompressed);
    delete compressed_table;
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

