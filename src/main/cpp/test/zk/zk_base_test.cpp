#include "zk_base_test.h"
#include <util/data_utilities.h>
#include <operators/sql_input.h>

using namespace vaultdb;

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);

const std::string ZkTest::unioned_db = "tpch_unioned_sf0.1";
const std::string ZkTest::alice_db = "tpch_unioned_sf0.1";
const std::string ZkTest::bob_db = "tpch_zk_bob"; // this should be an empty DBMS for testing but contain the same schema as tpc-h

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
    for(int i = 0; i < threads_; ++i)
        ios_[i] = new BoolIO<NetIO>(new NetIO(FLAGS_party == ALICE?nullptr: FLAGS_alice_host.c_str(),FLAGS_port+i), FLAGS_party==ALICE);

    setup_zk_bool<BoolIO<NetIO>>(ios_, threads_, FLAGS_party);
    // Alice gets unioned DB to query entire dataset for ZK Proof
    db_name = (FLAGS_party == ALICE) ? alice_db : bob_db;
    Utilities::mkdir("data");


}

void ZkTest::TearDown() {

    ios_[0]->io->flush();
    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    ASSERT_FALSE(cheat);

    for(int i = 0; i < threads_; ++i) {
        delete ios_[i]->io;
        delete ios_[i];
    }


}


shared_ptr<SecureTable> ZkTest::secret_share_input(const string & sql, bool has_dummy_tag, const SortDefinition & order_by)  const {
    ios_[0]->flush();


    // only alice can secret share in ZK land
    if(FLAGS_party == emp::ALICE) {
        SqlInput read_db(db_name, sql, has_dummy_tag, order_by);
        shared_ptr<PlainTable> input = read_db.run();

        uint32_t tuple_cnt = input->getTupleCount();
        ios_[0]->io->send_data(&tuple_cnt, 4);
        ios_[0]->io->flush();

        return PlainTable::secret_share_send_table(input, ios_[0] ->io, FLAGS_party);
    }
    else if(FLAGS_party == emp::BOB){ // BOB
        uint32_t tuple_cnt;
        ios_[0]->io->recv_data(&tuple_cnt, 4);
        ios_[0]->io->flush();

        PsqlDataProvider db_wrapper;

        shared_ptr<PlainTable> input = db_wrapper.getQueryTable(db_name, sql, has_dummy_tag);
        input->setSortOrder(order_by);


        // not input, just grabbing schema, maybe send this over the wire later

        QuerySchema schema = *input->getSchema();
        return PlainTable::secret_share_recv_table(schema, order_by, ios_[0]->io, emp::ALICE);
    }

    throw std::invalid_argument("Not a valid party for secret sharing!");
}
