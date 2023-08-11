#ifndef _ZK_BASE_TEST_H
#define _ZK_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <query_table/query_table.h>
#include <util/emp_manager/zk_manager.h>



namespace vaultdb {

    class ZkTest : public ::testing::Test {
    protected:

        void SetUp() override;

        void TearDown() override;


        static const std::string unioned_db_;
        static const std::string empty_db_;

        std::string db_name_ = unioned_db_; // default, set in setUp()
        ZKManager *manager_ = nullptr;

    private:


    };

}

#endif
