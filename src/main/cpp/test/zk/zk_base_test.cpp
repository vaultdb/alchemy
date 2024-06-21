#include "zk_base_test.h"
#include <util/data_utilities.h>
#include <operators/sql_input.h>
#include <util/field_utilities.h>
#include <util/logger.h>


using namespace vaultdb;
using namespace Logging;


const std::string ZkTest::unioned_db_ = "tpch_unioned_150";
const std::string ZkTest::empty_db_=  "tpch_empty"; // this should be an empty DBMS for testing but contain the same schema as tpc-h

//  set up tpch_zk_bob with:
//  \set target_db tpch_zk_bob
//DROP DATABASE IF EXISTS :target_db;
//CREATE DATABASE :target_db  WITH TEMPLATE tpch_unioned_int; -- or insert your source DB here
//\c :target_db
//        DELETE FROM lineitem CASCADE;
//DELETE FROM orders CASCADE;
//DELETE FROM customer CASCADE;
//DELETE FROM supplier CASCADE;
//DELETE FROM region CASCADE;
//DELETE FROM nation CASCADE;
//DELETE FROM part CASCADE;

void ZkTest::SetUp() {
    SystemConfiguration & s = SystemConfiguration::getInstance();
    storage_model_ = (FLAGS_storage == "row") ? StorageModel::ROW_STORE : StorageModel::COLUMN_STORE;
    s.setStorageModel(storage_model_);


    manager_ = new ZKManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);

    // Alice gets unioned DB to query entire dataset for ZK proof
    db_name_ = (FLAGS_party == ALICE) ? FLAGS_unioned_db : FLAGS_empty_db;
    Utilities::mkdir("data");
    s.emp_manager_ = manager_;

    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(FLAGS_unioned_db);
    s.initialize(db_name_, md, storage_model_, bp_parameters_);
    s.setEmptyDbName(empty_db_);
    s.emp_manager_ = manager_;
    s.emp_mode_ = EmpMode::ZK;
    Logger* log = get_log();

    log->write("ZkTest::SetUp() complete", Level::INFO);
   // log->write("DB name: " + db_name_, Level::INFO);

}

void ZkTest::TearDown() {

    manager_->flush();
    ASSERT_FALSE(manager_->finalize());

}


