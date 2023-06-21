#ifndef EMP_BASE_TEST_H
#define EMP_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h> // to ensure sh2pc manager works
#include <util/emp_manager/sh2pc_manager.h>
#include <query_table/query_table.h>
#include <util/logger.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);
DECLARE_string(storage); // row || column


using namespace vaultdb;

class EmpBaseTest  : public ::testing::Test {
protected:



    static const std::string unioned_db_;
    static const std::string alice_db_;
    static const std::string bob_db_;

    std::string db_name_ = unioned_db_; // default, set in setUp()
    StorageModel storage_model_ = StorageModel::ROW_STORE;
    SH2PCManager *manager_ = nullptr;

    void SetUp() override;
    void TearDown() override;
    void disableBitPacking();
    void initializeBitPacking(const string & unioned_db);

};


#endif //EMP_BASE_TEST_H
