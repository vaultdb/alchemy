#ifndef OMPC_BASE_TEST_H
#define OMPC_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <query_table/query_table.h>
#include <util/logger.h>
#include <util/emp_manager/emp_manager.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_int32(ctrl_port);
DECLARE_string(unioned_db);
DECLARE_string(empty_db);
DECLARE_int32(cutoff);
DECLARE_string(filter); //run only the tests with names matching this regex.
DECLARE_string(storage);
DECLARE_string(wires); // local path to wire files
DECLARE_int32(input_party); //run ./{binary_name} --gtest_list_tests to get all unit test names
DECLARE_int32(unpacked_page_size_bits);
DECLARE_int32(page_cnt);


using namespace vaultdb;

#if __has_include("emp-rescu/emp-rescu.h")

class OmpcBaseTest  : public ::testing::Test {
protected:


    static const std::string empty_db_;
    EmpMode emp_mode_;

    std::string db_name_; // set in setUp()
    StorageModel storage_model_ = StorageModel::COLUMN_STORE;
    EmpManager *manager_ = nullptr;
    vector<int> bp_parameters_ = {2048, 50, 5, 1000};
    vector<string> hosts_ = vector<string>(emp::N + 1, "127.0.0.1");
    int port_ = 54345;
    int ctrl_port_ = 65455;

    void SetUp() override;
    void TearDown() override;
    void disableBitPacking();
    void initializeBitPacking(const string & unioned_db);
    void parseIPsFromJson(const string &config_json_path);

};

#endif

#endif //OMPC_BASE_TEST_H
