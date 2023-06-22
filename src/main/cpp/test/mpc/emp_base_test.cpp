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
    std::cout << "Received storage flag of " << FLAGS_storage << '\n';
    std::cout << "Connecting to " << FLAGS_alice_host << " on port " << FLAGS_port << " as " << FLAGS_party << std::endl;

    manager_ = new SH2PCManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);
    db_name_ = (FLAGS_party == emp::ALICE) ? alice_db_ : bob_db_;

    // increment the port for each new test
    ++FLAGS_port;

    SystemConfiguration & s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(unioned_db_);
    s.initialize(db_name_, md, storage_model_);
    s.emp_manager_ = manager_;
}

void EmpBaseTest::TearDown() {
    manager_->flush();
    emp::finalize_semi_honest();
}



void EmpBaseTest::disableBitPacking() {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.clearBitPacking();
}

// e.g.,unioned db = tpch_unioned_250
// local db = tpch_alice_250
void EmpBaseTest::initializeBitPacking(const string &unioned_db) {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(unioned_db);
    s.initialize(unioned_db, md, storage_model_);
}


