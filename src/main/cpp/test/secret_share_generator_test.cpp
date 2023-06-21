#ifndef _SECRET_SHARE_GENERATOR_TEST_
#define _SECRET_SHARE_GENERATOR_TEST_

#include "plain/plain_base_test.h"
#include <util/data_utilities.h>
#include <query_table/query_table.h>
#include "query_table/table_factory.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");

// generates secret shares for a table - one for alice and one for bob
// then zips them together and verifies that it produces the same table

using namespace emp;
using namespace vaultdb;


class SecretShareGeneratorTest : public PlainBaseTest {


protected:

    PlainTable *assembleSecretShares(const QuerySchema & schema, const SecretShares & shares);
};

PlainTable *SecretShareGeneratorTest::assembleSecretShares(const QuerySchema &schema, const SecretShares & shares) {
    size_t share_cnt = shares.first.size();
    std::vector<int8_t> result_shares;
    result_shares.resize(share_cnt);

    int8_t *result = result_shares.data();
    const int8_t *aliceShares = shares.first.data();
    const int8_t *bobShares = shares.second.data();

    for(size_t i = 0; i < share_cnt; ++i) {
        result[i] = aliceShares[i] ^ bobShares[i];
    }

    return TableFactory<bool>::deserialize(schema, result_shares, storage_model_);

}

TEST_F(SecretShareGeneratorTest, lineitem_sample) {
    std::string query = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                             "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                             "FROM lineitem "
                             "ORDER BY l_shipdate "
                             "LIMIT 10";

    PlainTable *initial_table = DataUtilities::getQueryResults(db_name_, query, false);
    SecretShares secret_shares = initial_table->generateSecretShares();

    PlainTable *final_table = assembleSecretShares(initial_table->getSchema(), secret_shares);

    ASSERT_EQ(*initial_table, *final_table);
    delete initial_table;
    delete final_table;


}


TEST_F(SecretShareGeneratorTest, lineitem_dummy_tag_sample) {
    std::string query = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                        "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, l_shipinstruct <> 'NONE' AS dummy "
                        "FROM lineitem "
                        "ORDER BY l_shipdate "
                        "LIMIT 10";

    PlainTable *initial_table = DataUtilities::getQueryResults(db_name_, query, true);
    SecretShares secret_shares = initial_table->generateSecretShares();

    PlainTable *final_table = assembleSecretShares(initial_table->getSchema(), secret_shares);

    ASSERT_EQ(*initial_table, *final_table);
    delete initial_table;
    delete final_table;

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

#endif //  _SECRET_SHARE_GENERATOR_TEST_
