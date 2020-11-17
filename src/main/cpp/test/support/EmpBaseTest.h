#ifndef EMP_BASE_TEST_H
#define EMP_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <query_table/query_table.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);


using namespace vaultdb;

class EmpBaseTest  : public ::testing::Test {
protected:

    void SetUp() override;
    void TearDown() override;


    emp::NetIO *netio;
    static const std::string unionedDb;
    static const std::string aliceDb;
    static const std::string bobDb;

    std::string dbName = unionedDb; // default, set in setUp()

    static std::shared_ptr<QueryTable> getExpectedOutput(const std::string & sql, const int & sortColCount);


};


#endif //EMP_BASE_TEST_H
