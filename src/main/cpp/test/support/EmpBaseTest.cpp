#include <util/data_utilities.h>
#include "EmpBaseTest.h"


const std::string EmpBaseTest::unionedDb = "tpch_unioned";
const std::string EmpBaseTest::aliceDb = "tpch_alice";
const std::string EmpBaseTest::bobDb = "tpch_bob";

void EmpBaseTest::SetUp()  {

   //std::cout << "Connecting to " << FLAGS_alice_host << " on port " << FLAGS_port << " as " << FLAGS_party << std::endl;
    netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party,  1024*16);

    dbName = (FLAGS_party == emp::ALICE) ? aliceDb : bobDb;

    // increment the port for each new test
    ++FLAGS_port;

}

void EmpBaseTest::TearDown() {
        netio->flush();
        emp::finalize_semi_honest();
}

std::shared_ptr<PlainTable> EmpBaseTest::getExpectedOutput(const string &sql, const int &sortColCount) {
    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(EmpBaseTest::unionedDb, sql, false);
    SortDefinition expectedSortOrder = DataUtilities::getDefaultSortDefinition(sortColCount);
    expected->setSortOrder(expectedSortOrder);
    return expected;
}
