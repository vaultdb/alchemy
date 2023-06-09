#include <util/data_utilities.h>
#include "emp_base_test.h"
#include <util/logger.h>
#include <util/system_configuration.h>
#include "util/field_utilities.h"

const std::string EmpBaseTest::unioned_db_ = "tpch_unioned_150";
const std::string EmpBaseTest::alice_db_ = "tpch_alice_150";
const std::string EmpBaseTest::bob_db_ = "tpch_bob_150";

void EmpBaseTest::SetUp()  {
    assert(FLAGS_storage == "row" || FLAGS_storage == "column");
    storage_model_ = (FLAGS_storage == "row") ? StorageModel::ROW_STORE : StorageModel::COLUMN_STORE;

    Logger::setup(); // write to console
    string party_name = (FLAGS_party == 1) ? "alice"  : "bob";
    Logger::setup("vaultdb-" + party_name);

    std::cout << "Connecting to " << FLAGS_alice_host << " on port " << FLAGS_port << " as " << FLAGS_party << std::endl;

    netio_ =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio_, FLAGS_party, 1024 * 16);

    db_name_ = (FLAGS_party == emp::ALICE) ? alice_db_ : bob_db_;

    // increment the port for each new test
    ++FLAGS_port;
    start_time_ = clock_start();

    SystemConfiguration & s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(unioned_db_);

    s.initialize(db_name_, md);
}

void EmpBaseTest::TearDown() {
  double duration = time_from(start_time_) / 1e6;

	std::cout << "Runtime for test was " << duration << " secs." << std::endl;
        netio_->flush();
        delete netio_;
        emp::finalize_semi_honest();
}

PlainTable *EmpBaseTest::getExpectedOutput(const string &sql, const int &sort_col_cnt) const {

    StorageModel m = (FLAGS_storage == "row") ? StorageModel::ROW_STORE : StorageModel::COLUMN_STORE;
    PlainTable *expected = DataUtilities::getQueryResults(EmpBaseTest::unioned_db_, sql, m, false);
    SortDefinition expectedSortOrder = DataUtilities::getDefaultSortDefinition(sort_col_cnt);
    expected->setSortOrder(expectedSortOrder);
    return expected;
}
