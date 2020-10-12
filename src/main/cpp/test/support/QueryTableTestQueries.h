//
// Created by Jennie Rogers on 10/7/20.
//

#ifndef _QUERY_TABLE_TEST_QUERIES_H
#define _QUERY_TABLE_TEST_QUERIES_H

#include <gtest/gtest.h>
#include <vaultdb.h>


class QueryTableTestQueries : public ::testing::Environment {
public:
    void SetUp() override {}

    void TearDown() override {}

    // Assume there's only going to be a single instance of this class, so we can just
    // hold the timestamp as a const static local variable and expose it through a
    // static member function
    static std::string getInputQuery();
    static std::unique_ptr<QueryTable> getExpectedSecureOutput();

    static std::string getInputQueryDummyTag();

    static std::unique_ptr<QueryTable> getExpectedSecureOutputDummyTag();




};



#endif //VAULTDB_EMP_QUERYTABLETESTQUERIES_H
