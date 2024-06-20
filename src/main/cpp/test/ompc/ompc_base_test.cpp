#include <util/data_utilities.h>
#include "ompc_base_test.h"
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include <util/logger.h>

#include "query_table/secure_tuple.h" // for use in child classes
#include "query_table/plain_tuple.h"

#if __has_include("emp-rescu/emp-rescu.h")

const std::string OmpcBaseTest::empty_db_ = "tpch_empty";

using namespace Logging;

void OmpcBaseTest::SetUp()  {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ =  EmpMode::OUTSOURCED;
    emp_mode_ =  s.emp_mode_;
    // defaults to column store
    assert(FLAGS_storage == "column" || FLAGS_storage == "wire_packed");
    if(FLAGS_storage == "wire_packed") {
        storage_model_ = StorageModel::PACKED_COLUMN_STORE;
    }
    else {
        storage_model_ = StorageModel::COLUMN_STORE;
    }

    s.setStorageModel(storage_model_);
    s.input_party_ = FLAGS_input_party;
	Logger* log = get_log();

    log->write("Connecting on ports " + std::to_string(FLAGS_port) + ", " + std::to_string(FLAGS_ctrl_port) + " as " + std::to_string(FLAGS_party), Level::INFO);



//        string hosts[] = { // codd2
//                          "129.105.61.179", // codd5 (Alice)
//                          "129.105.61.184", // codd10 (Bob)
//                          "129.105.61.186", // codd12 (Carol)
//                          "129.105.61.176" // codd2 (TP)
//                           };
        string hosts[] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
        // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
        manager_ = new OutsourcedMpcManager(hosts, FLAGS_party, FLAGS_port, FLAGS_ctrl_port);
        db_name_ = (FLAGS_party == emp::TP) ? FLAGS_unioned_db : empty_db_;
        
        FLAGS_port += N;
        FLAGS_ctrl_port += N;




    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(FLAGS_unioned_db);
    s.initialize(db_name_, md, storage_model_);
    string settings = Utilities::getTestParameters();
    log->write(settings, Level::INFO);

}

void OmpcBaseTest::TearDown() {
    manager_->flush();
    delete manager_;
    SystemConfiguration::getInstance().emp_manager_ = nullptr;
}



void OmpcBaseTest::disableBitPacking() {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.clearBitPacking();
}

// e.g.,unioned db = tpch_unioned_250
// local db = tpch_alice_250
void OmpcBaseTest::initializeBitPacking(const string &unioned_db) {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(unioned_db);
    s.initialize(unioned_db, md, storage_model_);
}


#endif
