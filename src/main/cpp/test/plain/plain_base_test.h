#ifndef _PLAIN_BASE_TEST_H
#define _PLAIN_BASE_TEST_H

#include <stdexcept>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/logger.h>
#include <util/utilities.h>
#include "common/defs.h"
#include <util/emp_manager/emp_manager.h>

DECLARE_int32(cutoff);
DECLARE_string(filter);
DECLARE_string(storage);

using namespace vaultdb;
using namespace emp;


class PlainBaseTest  : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    std::string current_working_directory_;
    const string db_name_ = "tpch_unioned_1500";
    StorageModel storage_model_ = StorageModel::COLUMN_STORE;
    EmpManager *manager_ = nullptr;


};


#endif //_PLAIN_BASE_TEST_H
