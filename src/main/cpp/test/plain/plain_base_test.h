#ifndef _PLAIN_BASE_TEST_H
#define _PLAIN_BASE_TEST_H

#include <stdexcept>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/logger.h>
#include <util/utilities.h>


using namespace vaultdb;
using namespace emp;


class PlainBaseTest  : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    std::string current_working_directory_;
    const string db_name_ = "tpch_unioned_150";


};


#endif //_PLAIN_BASE_TEST_H
