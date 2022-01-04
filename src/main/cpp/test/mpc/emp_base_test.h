#ifndef EMP_BASE_TEST_H
#define EMP_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <query_table/query_table.h>
#include <util/logger.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);

// ignore bob's inputs for emp eval
#define IGNORE_BOB 1

using namespace vaultdb;

class EmpBaseTest  : public ::testing::Test {
protected:

    void SetUp() override;
    void TearDown() override;


    emp::NetIO *netio_;
    static const std::string unioned_db_;
    static const std::string alice_db_;
    static const std::string bob_db_;

    std::string db_name_ = unioned_db_; // default, set in setUp()

    static std::shared_ptr<PlainTable> getExpectedOutput(const std::string & sql, const int & sortColCount);


};


#endif //EMP_BASE_TEST_H
