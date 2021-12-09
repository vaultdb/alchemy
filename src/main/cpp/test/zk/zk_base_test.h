#ifndef VAULTDB_EMP_ZK_BASE_TEST_H
#define VAULTDB_EMP_ZK_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <emp-zk/emp-zk.h>
#include <query_table/query_table.h>



namespace vaultdb {

    class ZkTest : public ::testing::Test {
    protected:

        void SetUp() override;

        void TearDown() override;

        shared_ptr<SecureTable> secret_share_input(const string & sql, bool has_dummy_tag, const SortDefinition & order_by = SortDefinition()) const;

        static const std::string unioned_db;
        static const std::string alice_db;
        static const std::string bob_db;

        std::string db_name = unioned_db; // default, set in setUp()
        static const int threads = 1;
        BoolIO<NetIO> *ios[threads];

    };

}

#endif