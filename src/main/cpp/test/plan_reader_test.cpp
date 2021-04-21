#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <util/plan_reader.h>

using namespace emp;
using namespace vaultdb;


class PlanReaderTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };
    void TearDown() override{
        finalize_plain_prot();
    };
    const string db_name = "tpch_unioned"; // plaintext case first

};


TEST_F(PlanReaderTest, tpch_q1) {
    string test_name = "q1";
    PlanReader<bool> plan_reader(db_name, test_name);
}