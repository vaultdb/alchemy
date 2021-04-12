#ifndef VAULTDB_EMP_ZKBASETEST_H
#define VAULTDB_EMP_ZKBASETEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <emp-zk/emp-zk.h>


namespace vaultdb {

    class ZkBaseTest : public ::testing::Test {
    protected:

        void SetUp() override;

        void TearDown() override;

        static const std::string unioned_db;
        static const std::string alice_db;
        static const std::string bob_db;

        std::string db_name = unioned_db; // default, set in setUp()
        static const int threads = 1;
        BoolIO<NetIO> *ios[threads];

    };

}

#endif