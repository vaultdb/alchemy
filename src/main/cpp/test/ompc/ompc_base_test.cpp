#include <util/data_utilities.h>
#include "ompc_base_test.h"
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include <util/logger.h>

#include <boost/property_tree/json_parser.hpp>

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

    // parse IPs and ports from config.json
    parseIPsFromJson(Utilities::getCurrentWorkingDirectory() + "/config.json");

    log->write("Connecting on ports " + std::to_string(port_) + ", " + std::to_string(ctrl_port_) + " as " + std::to_string(FLAGS_party), Level::INFO);

    string hosts[] = {alice_host_, bob_host_, carol_host_, trusted_party_host_};
    // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
    manager_ = new OutsourcedMpcManager(hosts, FLAGS_party, port_, ctrl_port_);
    db_name_ = (FLAGS_party == emp::TP) ? FLAGS_unioned_db : empty_db_;

    port_ += N;
    ctrl_port_ += N;

    // set bp parameters with gflags
    bp_parameters_[0] = FLAGS_unpacked_page_size_bits;
    bp_parameters_[1] = FLAGS_page_cnt;

    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(FLAGS_unioned_db);
    s.initialize(db_name_, md, storage_model_, bp_parameters_);
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
    s.initialize(unioned_db, md, storage_model_, bp_parameters_);
}

void OmpcBaseTest::parseIPsFromJson(const std::string &config_json_path) {
    stringstream ss;
    std::vector<std::string> json_lines = DataUtilities::readTextFile(config_json_path);
    for(vector<string>::iterator pos = json_lines.begin(); pos != json_lines.end(); ++pos)
        ss << *pos << endl;

    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    if(pt.count("hosts") > 0) {
        boost::property_tree::ptree hosts = pt.get_child("hosts");

        for(ptree::const_iterator it = hosts.begin(); it != hosts.end(); ++it) {
            if(it->second.count("1") > 0) {
                alice_host_ = it->second.get_child("1").get_value<std::string>();
            }
            else {
                throw std::runtime_error("No alice host found in config.json");
            }

            if(it->second.count("2") > 0) {
                bob_host_ = it->second.get_child("2").get_value<std::string>();
            }
            else {
                throw std::runtime_error("No bob host found in config.json");
            }

            if(it->second.count("3") > 0) {
                carol_host_ = it->second.get_child("3").get_value<std::string>();
            }
            else {
                throw std::runtime_error("No carol host found in config.json");
            }

            if(it->second.count("10086") > 0) {
                trusted_party_host_ = it->second.get_child("10086").get_value<std::string>();
            }
            else {
                throw std::runtime_error("No tp host found in config.json");
            }
        }

        port_ = pt.get_child("port").get_value<int32_t>();
        ctrl_port_ = pt.get_child("ctrl_port").get_value<int32_t>();
    }
    else {
        throw std::runtime_error("No hosts found in config.json");
    }
}


#endif
