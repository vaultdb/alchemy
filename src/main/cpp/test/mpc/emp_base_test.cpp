#include <util/data_utilities.h>
#include "emp_base_test.h"
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/emp_manager/sh2pc_outsourced_manager.h"
#include "util/emp_manager/zk_manager.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "query_table/secure_tuple.h" // for use in child classes
#include "query_table/plain_tuple.h"

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
    if(FLAGS_storage == "wire_packed") {
        storage_model_ = StorageModel::PACKED_COLUMN_STORE;
    }
    else if(FLAGS_storage == "compressed") {
        storage_model_ = StorageModel::COMPRESSED_STORE;
    }

    s.setStorageModel(storage_model_);

	Logger* log = get_log();

    std::stringstream ss;

    ss << "Received emp mode: " << EmpManager::empModeString(emp_mode_) << endl;

    log->write(ss.str(), Level::INFO);
    log->write("Connecting to " + FLAGS_alice_host + " on ports " + std::to_string(FLAGS_port) + ", " + std::to_string(FLAGS_ctrl_port) + " as " + std::to_string(FLAGS_party), Level::INFO);


    if(emp_mode_ == EmpMode::OUTSOURCED) { // host_list = {alice, bob, carol, trusted party}

//        string hosts[] = { // codd2
//                          "129.105.61.179", // codd5 (Alice)
//                          "129.105.61.184", // codd10 (Bob)
//                          "129.105.61.186", // codd12 (Carol)
//                          "129.105.61.176" // codd2 (TP)
//                           };
        string hosts[] = {FLAGS_alice_host, FLAGS_alice_host, FLAGS_alice_host, FLAGS_alice_host};
        // to enable wire packing set storage model to StorageModel::PACKED_COLUMN_STORE
        //s.setStorageModel(StorageModel::PACKED_COLUMN_STORE);
        manager_ = new OutsourcedMpcManager(hosts, FLAGS_party, FLAGS_port, FLAGS_ctrl_port);
        db_name_ = (FLAGS_party == emp::TP) ? FLAGS_unioned_db : empty_db_;
        
        FLAGS_port += N;
        FLAGS_ctrl_port += N;
    }
    else if(emp_mode_ == EmpMode::SH2PC) {
        assert(storage_model_ != StorageModel::PACKED_COLUMN_STORE);
        if(storage_model_ == StorageModel::COMPRESSED_STORE) {
            manager_ = new SH2PCOutsourcedManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);
            db_name_ = (FLAGS_party == ALICE) ? FLAGS_unioned_db : empty_db_;
        }
        else {
            manager_ = new SH2PCManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);
            db_name_ = (FLAGS_party == emp::ALICE) ? FLAGS_alice_db : FLAGS_bob_db;
        }
        // increment the port for each new test
        ++FLAGS_port;
		++FLAGS_ctrl_port;
    }
    else if(emp_mode_ == EmpMode::ZK) {
        assert(storage_model_ != StorageModel::PACKED_COLUMN_STORE);
        manager_ = new ZKManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);

        // Alice gets unioned DB to query entire dataset for ZK proof
        db_name_ = (FLAGS_party == ALICE) ? FLAGS_unioned_db : empty_db_;
        Utilities::mkdir("data");
        s.emp_manager_ = manager_; // probably not needed
    }
    else {
        throw std::runtime_error("No EMP backend found.");
    }


    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(FLAGS_unioned_db);
    s.initialize(db_name_, md, storage_model_);

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


