//
// Created by Jennie Rogers on 10/24/20.
//


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

};


#endif //EMP_BASE_TEST_H
