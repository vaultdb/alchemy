//
// Created by madhav on 12/27/19.
//
#include "pqxx_compat.h"
#include <gtest/gtest.h>

class pqxx_compat_test : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override{};
};

// TODO(madhavsuresh): build out these tests
TEST_F(pqxx_compat_test, get_schema) {
  auto res = query("dbname=tpch_sf1", "SELECT * FROM customer LIMIT 10");
  auto schema = GetSchemaFromQuery(res);
  std::cout << schema.DebugString();
}

TEST_F(pqxx_compat_test, GetPqTable) {
  dbquery::Table t =
      GetPqTable("dbname=tpch_sf1", "SELECT l_orderkey FROM lineitem LIMIT 10");
  std::cout << t.DebugString();
}
