#include <util/data_utilities.h>
#include "ompc_base_test.h"
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include <util/logger.h>
#include "util/parsing_utilities.h"

#include <boost/property_tree/json_parser.hpp>

#if __has_include("emp-rescu/emp-rescu.h")

const std::string OmpcBaseTest::empty_db_ = "tpch_empty";

using namespace Logging;

void OmpcBaseTest::SetUp()  {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    emp_mode_ = s.emp_mode_ =  EmpMode::OUTSOURCED;

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
    s.bp_enabled_ = true;

	Logger* log = get_log();

    // parse IPs and ports from config.json
    ConnectionInfo c = ParsingUtilities::parseIPsFromJson(Utilities::getCurrentWorkingDirectory() + "/conf/config.json");
    for(auto & entry : c.hosts_) {
        hosts_.push_back(entry.second);
    }
    port_ = c.port_;
    ctrl_port_ = c.ctrl_port_;

    log->write("Connecting on ports " + std::to_string(port_) + ", " + std::to_string(ctrl_port_) + " as " + std::to_string(FLAGS_party), Level::INFO);

    string data_path = Utilities::getCurrentWorkingDirectory() + "/" + ((storage_model_ == StorageModel::PACKED_COLUMN_STORE) ? FLAGS_wire_path : "shares") + "/" + FLAGS_unioned_db;
    manager_ = new OutsourcedMpcManager(hosts_.data(), FLAGS_party, port_, ctrl_port_, data_path + "/backend_state." +
            to_string(FLAGS_party));
    db_name_ = (FLAGS_party == emp::TP) ? FLAGS_unioned_db : empty_db_;

    port_ += N;
    ctrl_port_ += N;

    // set bp parameters with gflags
    bp_unpacked_page_size_bits_  = FLAGS_unpacked_page_size_bits;
    bp_page_cnt_ = FLAGS_page_cnt;

    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;

    s.initialize(db_name_, BitPackingMetadata(), storage_model_, bp_unpacked_page_size_bits_, bp_page_cnt_);
    string settings = Utilities::getTestParameters();
    log->write(settings, Level::INFO);

    if(storage_model_ == StorageModel::PACKED_COLUMN_STORE){
        // read in files for packed wires
        s.initializeWirePackedDb(data_path);
    }
    else {
        s.initializeOutsourcedSecretShareDb(data_path);
    }
}

void OmpcBaseTest::TearDown() {
    manager_->flush();
    delete manager_;
    SystemConfiguration::getInstance().emp_manager_ = nullptr;
}



// e.g.,unioned db = tpch_unioned_250
// local db = tpch_alice_250
void OmpcBaseTest::initializeBitPacking(const string &unioned_db) {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(unioned_db);
    s.initialize(unioned_db, md, storage_model_, bp_unpacked_page_size_bits_, bp_page_cnt_);
}




#endif
