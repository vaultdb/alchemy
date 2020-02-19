//
// Created by madhav on 12/28/19.
//
#include "pqxx_compat.h"
#include "proto_converter.h"
#include <gtest/gtest.h>

class proto_converter_test : public ::testing::Test {
protected:
  void SetUp() override{};

  void TearDown() override{};
};

TEST_F(proto_converter_test, query_table_conversion) {
  auto table = GetPqTable(
      "dbname=tpch_sf1", "SELECT l_orderkey, l_partkey FROM lineitem LIMIT 10");
  auto t = ProtoToQuerytable(table);
  // qf->get_as<emp::Bit **>();
}