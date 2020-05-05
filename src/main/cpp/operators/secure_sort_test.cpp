//
// Created by Salome Kariuki on 4/29/20.
//
#include <gtest/gtest.h>
#include "secure_sort.h"
#include <data/PQDataProvider.h>
#include <ctime>

class secure_join_test : public ::testing::Test {
protected:
    void SetUp() override {};
    void TearDown() override{};
};

TEST_F(secure_sort_test, simple_execution_test) {
PQDataProvider pq;
auto qt = pq.GetQueryTable("dbname=tpch_sf1", "SELECT * FROM customer LIMIT 10");
vector<int> ordinals{1,2,3};
SortDef def;
def.order = SortOrder::ASCENDING;
def.ordinals = ordinals;
clock_t time_req;
time_req = clock();
Sort(qt.get(), def);
time_req = clock() - time_req;
std::cout << "Running sort operation took: " << time_req << std::endl;
}