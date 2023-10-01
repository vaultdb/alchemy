#ifndef EMP_BASE_TEST_H
#define EMP_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <query_table/query_table.h>
#include <util/logger.h>
#include <util/emp_manager/emp_manager.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_int32(ctrl_port);
DECLARE_string(alice_host);
DECLARE_string(unioned_db);
DECLARE_string(alice_db);
DECLARE_string(bob_db);
DECLARE_int32(cutoff);
DECLARE_string(storage); // row || column
DECLARE_string(filter); //run only the tests with names matching this regex.
						//run ./{binary_name} --gtest_list_tests to get all unit test names


using namespace vaultdb;

class EmpBaseTest  : public ::testing::Test {
protected:


    static const std::string empty_db_;
    EmpMode emp_mode_;

    std::string db_name_; // set in setUp()
    StorageModel storage_model_ = StorageModel::ROW_STORE;
    EmpManager *manager_ = nullptr;

    void SetUp() override;
    void TearDown() override;
    void disableBitPacking();
    void initializeBitPacking(const string & unioned_db);

};


#endif //EMP_BASE_TEST_H
