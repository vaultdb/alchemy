#include "zk_base_test.h"
#include <util/data_utilities.h>
#include <operators/sql_input.h>

using namespace vaultdb;

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);

const std::string ZkTest::unioned_db_ = "tpch_unioned_15000"; // rename from tpch_unioned_sf0.1 to avoid non-standard characters
const std::string ZkTest::empty_db_=  "tpch_zk_bob"; // this should be an empty DBMS for testing but contain the same schema as tpc-h

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
    manager_ = new ZKManager(FLAGS_alice_host, FLAGS_party, FLAGS_port);

    // Alice gets unioned DB to query entire dataset for ZK Proof
    db_name_ = (FLAGS_party == ALICE) ? unioned_db_ : empty_db_;
    Utilities::mkdir("data");

}

void ZkTest::TearDown() {

    manager_->flush();
    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    ASSERT_FALSE(cheat);

}


//SecureTable *ZkTest::secret_share_input(const string & sql, bool has_dummy_tag, const SortDefinition & order_by)  const {
//    manager_->flush();
//
//
//    // only alice can secret share in ZK land
//    if(FLAGS_party == emp::ALICE) {
//
//        SqlInput read_db(unioned_db_, sql, false, order_by);
//        PlainTable *input = read_db.run();
//        return input->secretShare()->clone();
//    }
//    else if(FLAGS_party == emp::BOB){ // BOB
//        uint32_t tuple_cnt;
//        ios_[0]->io->recv_data(&tuple_cnt, 4);
//        ios_[0]->io->flush();
//
//        PsqlDataProvider db_wrapper;
//
//        shared_ptr<PlainTable> input = db_wrapper.getQueryTable(db_name, sql);
//        input->setSortOrder(order_by);
//
//
//        // not input, just grabbing schema, maybe send this over the wire later
//
//        QuerySchema schema = *input->getSchema();
//        return PlainTable::secret_share_recv_table(schema, order_by, ios_[0]->io, emp::ALICE);
//    }
//
//    throw std::invalid_argument("Not a valid party for secret sharing!");
//}
