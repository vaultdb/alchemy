//
// Created by madhav on 1/28/20.
//
#include <gtest/gtest.h>
#include "secure_join.h"
#include <data/PQDataProvider.h>
class secure_join_test : public ::testing::Test {
protected:
  void SetUp() override {};
  void TearDown() override{};
};

TEST_F(secure_join_test, simple_execution_test) {
  PQDataProvider pq;
  auto qt = pq.GetQueryTable("dbname=tpch_sf1", "SELECT l_orderkey FROM lineitem LIMIT 10");
  JoinDef def;
  def.id = vaultdb::expression::ExpressionId::EQUAL;
  def.right_index = 0;
  def.left_index = 0;
    Join(qt.get(), qt.get(), def);
}
