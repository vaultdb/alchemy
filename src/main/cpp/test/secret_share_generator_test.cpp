#ifndef _SECRET_SHARE_GENERATOR_TEST_
#define _SECRET_SHARE_GENERATOR_TEST_

#include <util/data_utilities.h>
#include <gtest/gtest.h>
#include <query_table/query_table.h>


// generates secret shares for a table - one for alice and one for bob
// then zips them together and verifies that it produces the same table

using namespace emp;
using namespace vaultdb::types;
using namespace vaultdb;


class SecretShareGeneratorTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };

    void TearDown() override{
        finalize_plain_prot();
    };

    const std::string dbName = "tpch_unioned";

    std::shared_ptr<QueryTable> assembleSecretShares(const QuerySchema & schema, const SecretShares & shares);
};

std::shared_ptr<QueryTable> SecretShareGeneratorTest::assembleSecretShares(const QuerySchema &schema, const SecretShares & shares) {
    size_t shareCount = shares.first.size();
    std::vector<int8_t> resultShares;
    resultShares.resize(shareCount);

    int8_t *result = resultShares.data();
    const int8_t *aliceShares = shares.first.data();
    const int8_t *bobShares = shares.second.data();

    for(size_t i = 0; i < shareCount; ++i) {
        result[i] = aliceShares[i] ^ bobShares[i];
    }

    return QueryTable::deserialize(schema, resultShares);

}

TEST_F(SecretShareGeneratorTest, lineitem_sample) {
    std::string query = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                             "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                             "FROM lineitem "
                             "ORDER BY l_shipdate "
                             "LIMIT 10";

    std::shared_ptr<QueryTable> initialTable = DataUtilities::getQueryResults(dbName, query, false);
    SecretShares secretShares = initialTable->generateSecretShares();

    std::shared_ptr<QueryTable> finalTable = assembleSecretShares(initialTable->getSchema(), secretShares);

    ASSERT_EQ(*initialTable, *finalTable);

}


TEST_F(SecretShareGeneratorTest, lineitem_dummy_tag_sample) {
    std::string query = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                        "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, l_shipinstruct <> 'NONE' AS dummy "
                        "FROM lineitem "
                        "ORDER BY l_shipdate "
                        "LIMIT 10";

    std::shared_ptr<QueryTable> initialTable = DataUtilities::getQueryResults(dbName, query, true);
    SecretShares secretShares = initialTable->generateSecretShares();

    std::shared_ptr<QueryTable> finalTable = assembleSecretShares(initialTable->getSchema(), secretShares);

    ASSERT_EQ(*initialTable, *finalTable);

}

#endif //  _SECRET_SHARE_GENERATOR_TEST_
