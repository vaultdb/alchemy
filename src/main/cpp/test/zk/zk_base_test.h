#ifndef _ZK_BASE_TEST_H
#define _ZK_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <query_table/query_table.h>
#include <util/emp_manager/zk_manager.h>


DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);
DECLARE_string(unioned_db);
DECLARE_string(empty_db);
DECLARE_string(storage); // row || column
DECLARE_string(filter); //run only the tests with names matching this regex.


namespace vaultdb {

    class ZkTest : public ::testing::Test {
    protected:

        void SetUp() override;

        void TearDown() override;


        static const std::string unioned_db_;
        static const std::string empty_db_;
        StorageModel storage_model_;

        std::string db_name_ = unioned_db_; // default, set in setUp()
        ZKManager *manager_ = nullptr;
        vector<int> bp_parameters_ = {2048, 50, 5, 1000};

    private:


    };

}

#endif
