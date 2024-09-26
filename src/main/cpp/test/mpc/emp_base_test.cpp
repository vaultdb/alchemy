#include <util/data_utilities.h>
#include "emp_base_test.h"
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/emp_manager/sh2pc_outsourced_manager.h"
#include "util/emp_manager/zk_manager.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include <util/logger.h>

#include "query_table/secure_tuple.h" // for use in child classes
#include "query_table/plain_tuple.h"

#include <boost/property_tree/json_parser.hpp>

#if  __has_include("emp-sh2pc/emp-sh2pc.h")
    static EmpMode _emp_mode_ = EmpMode::SH2PC;
#elif __has_include("emp-zk/emp-zk.h")
    static EmpMode _emp_mode_ = EmpMode::ZK;
#elif __has_include("emp-rescu/emp-rescu.h")
    static EmpMode _emp_mode_ = EmpMode::OUTSOURCED;
#else
    static EmpMode _emp_mode_ = EmpMode::PLAIN;
#endif

const std::string EmpBaseTest::empty_db_ = "tpch_empty";

using namespace Logging;

void EmpBaseTest::SetUp()  {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ =  _emp_mode_;
    emp_mode_ =  _emp_mode_;
    // defaults to column store
    assert(FLAGS_storage == "column" || FLAGS_storage == "wire_packed" || FLAGS_storage == "compressed");
    if(FLAGS_storage == "wire_packed") {
        storage_model_ = StorageModel::PACKED_COLUMN_STORE;
    }
    else if(FLAGS_storage == "compressed") {
        storage_model_ = StorageModel::COMPRESSED_STORE;
    }

    s.setStorageModel(storage_model_);

    std::string alice_host;
    std::string bob_host;
    std::string carol_host;
    std::string trusted_party_host;
    int port;
    int ctrl_port;

    std::string config_json_path = Utilities::getCurrentWorkingDirectory() + "/conf/config.json";

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
                alice_host = it->second.get_child("1").get_value<std::string>();
            }
            else {
                throw std::runtime_error("No alice host found in config.json");
            }

            if(emp_mode_ == EmpMode::SH2PC || emp_mode_ == EmpMode::ZK) {
                if(it->second.count("2") > 0) {
                    bob_host = it->second.get_child("2").get_value<std::string>();
                }
                else {
                    throw std::runtime_error("No bob host found in config.json");
                }
            }

            if(emp_mode_ == EmpMode::OUTSOURCED) {
                if(it->second.count("2") > 0) {
                    bob_host = it->second.get_child("2").get_value<std::string>();
                }
                else {
                    throw std::runtime_error("No bob host found in config.json");
                }

                if(it->second.count("3") > 0) {
                    carol_host = it->second.get_child("3").get_value<std::string>();
                }
                else {
                    throw std::runtime_error("No carol host found in config.json");
                }

                if(it->second.count("10086") > 0) {
                    trusted_party_host = it->second.get_child("10086").get_value<std::string>();
                }
                else {
                    throw std::runtime_error("No tp host found in config.json");
                }
            }
        }

        port = pt.get_child("port").get_value<int32_t>();
        ctrl_port = pt.get_child("ctrl_port").get_value<int32_t>();
    }
    else {
        throw std::runtime_error("No hosts found in config.json");
    }

	Logger* log = get_log();

    log->write("Connecting to " + FLAGS_alice_host + " on ports " + std::to_string(FLAGS_port) + ", " + std::to_string(FLAGS_ctrl_port) + " as " + std::to_string(FLAGS_party), Level::INFO);


    if(emp_mode_ == EmpMode::OUTSOURCED) { // host_list = {alice, bob, carol, trusted party}
        string hosts[] = {alice_host, bob_host, carol_host, trusted_party_host};

        // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
        manager_ = new OutsourcedMpcManager(hosts, FLAGS_party, port, ctrl_port);
        db_name_ = (FLAGS_party == emp::TP) ? FLAGS_unioned_db : empty_db_;

        port += N;
        ctrl_port += N;
    }
    else if(emp_mode_ == EmpMode::SH2PC) {
        assert(storage_model_ != StorageModel::PACKED_COLUMN_STORE);
        if(storage_model_ == StorageModel::COMPRESSED_STORE) {
            manager_ = new SH2PCOutsourcedManager(alice_host, FLAGS_party, port);
            emp_mode_ = vaultdb::EmpMode::SH2PC_OUTSOURCED;
            db_name_ = (FLAGS_party == ALICE) ? FLAGS_unioned_db : empty_db_;
        }
        else {
            manager_ = new SH2PCManager(alice_host, FLAGS_party, port);
            db_name_ = (FLAGS_party == emp::ALICE) ? FLAGS_alice_db : FLAGS_bob_db;
        }
        // increment the port for each new test
        ++port;
        ++ctrl_port;
    }
    else if(emp_mode_ == EmpMode::ZK) {
        assert(storage_model_ != StorageModel::PACKED_COLUMN_STORE);
        manager_ = new ZKManager(alice_host, FLAGS_party, port);

        // Alice gets unioned DB to query entire dataset for ZK proof
        db_name_ = (FLAGS_party == ALICE) ? FLAGS_unioned_db : empty_db_;
        Utilities::mkdir("data");
        s.emp_manager_ = manager_; // probably not needed
        // increment the port for each new test
        ++port;
        ++ctrl_port;
    }
    else {
        throw std::runtime_error("No EMP backend found.");
    }




    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(FLAGS_unioned_db);
    s.initialize(db_name_, md, storage_model_);
    string settings = Utilities::getTestParameters();
    log->write(settings, Level::INFO);

}

void EmpBaseTest::TearDown() {
    manager_->flush();
    if(emp_mode_ == EmpMode::ZK) {
        ZKManager *mgr = (ZKManager *) manager_;
        ASSERT_FALSE(mgr->finalize());
        delete mgr;
    }
    else {
        delete manager_;
    }
    SystemConfiguration::getInstance().emp_manager_ = nullptr;
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


