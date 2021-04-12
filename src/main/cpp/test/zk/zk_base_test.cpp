#include "zk_base_test.h"
#include <util/data_utilities.h>
#include <operators/sql_input.h>

using namespace vaultdb;

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);

const std::string ZkTest::unioned_db = "tpch_unioned";
const std::string ZkTest::alice_db = "tpch_alice";
const std::string ZkTest::bob_db = "tpch_bob";


void ZkTest::SetUp() {
    for(int i = 0; i < threads; ++i)
        ios[i] = new BoolIO<NetIO>(new NetIO(FLAGS_party == ALICE?nullptr: FLAGS_alice_host.c_str(),FLAGS_port+i), FLAGS_party==ALICE);

    setup_zk_bool<BoolIO<NetIO>>(ios, threads, FLAGS_party);
    db_name = (FLAGS_party == ALICE) ? alice_db : bob_db;

}

void ZkTest::TearDown() {

    ios[0]->io->flush();
    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    ASSERT_FALSE(cheat);

    for(int i = 0; i < threads; ++i) {
        delete ios[i]->io;
        delete ios[i];
    }


}


shared_ptr<SecureTable> ZkTest::secret_share_input(const string & sql, bool has_dummy_tag, const SortDefinition & order_by)  const {


    // only alice can secret share in ZK land
    if(FLAGS_party == emp::ALICE) {
        SqlInput read_db(db_name, sql, has_dummy_tag, order_by);
        shared_ptr<PlainTable> input = read_db.run();
        uint32_t tuple_cnt = input->getTupleCount();
        ios[0]->io->send_data(&tuple_cnt, 4);
        ios[0]->io->flush();

        return PlainTable::secret_share_send_table(input, FLAGS_party);
    }
    else if(FLAGS_party == emp::BOB){ // BOB
        uint32_t tuple_cnt;
        ios[0]->io->recv_data(&tuple_cnt, 4);

        // not input, just grabbing schema, maybe send this over the wire later
        SqlInput read_db(db_name, sql, has_dummy_tag);
        shared_ptr<PlainTable> input = read_db.run();

        QuerySchema schema = *input->getSchema();
        return PlainTable ::secret_share_recv_table(tuple_cnt, schema, order_by, emp::ALICE);
    }

    throw std::invalid_argument("Not a valid party for secret sharing!");
}
