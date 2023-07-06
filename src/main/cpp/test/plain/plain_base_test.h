#ifndef _PLAIN_BASE_TEST_H
#define _PLAIN_BASE_TEST_H

#include <stdexcept>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/logger.h>
#include <util/utilities.h>
#include "common/defs.h"
#include <util/emp_manager/emp_manager.h>

DECLARE_string(storage); // row || column
DECLARE_string(filter);

using namespace vaultdb;
using namespace emp;


class PlainBaseTest  : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    std::string current_working_directory_;
    const string db_name_ = "tpch_unioned_150";
    StorageModel storage_model_ = StorageModel::ROW_STORE;
    EmpManager *manager_ = nullptr;


};


#endif //_PLAIN_BASE_TEST_H
