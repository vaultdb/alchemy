#ifndef EMP_BASE_TEST_H
#define EMP_BASE_TEST_H
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);



class EmpBaseTest  : public ::testing::Test {
protected:

    void SetUp() override;
    void TearDown() override;


    emp::NetIO *netio;
    const std::string unionedDb = "tpch_unioned";
    const std::string aliceDb = "tpch_alice";
    const std::string bobDb = "tpch_bob";

    std::string dbName = unionedDb; // default, set in setUp()


    //static void SetUpTestCase();
    //static void TearDownTestCase();
    //static emp::NetIO *netio;


};


#endif //EMP_BASE_TEST_H
