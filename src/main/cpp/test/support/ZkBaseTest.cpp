#include "ZkBaseTest.h"

using namespace vaultdb;

DECLARE_int32(party);
DECLARE_int32(port);
DECLARE_string(alice_host);

const std::string ZkBaseTest::unioned_db = "tpch_unioned";
const std::string ZkBaseTest::alice_db = "tpch_alice";
const std::string ZkBaseTest::bob_db = "tpch_bob";


void ZkBaseTest::SetUp() {
    for(int i = 0; i < threads; ++i)
        ios[i] = new BoolIO<NetIO>(new NetIO(FLAGS_party == ALICE?nullptr: FLAGS_alice_host.c_str(),FLAGS_port+i), FLAGS_party==ALICE);

    setup_zk_bool<BoolIO<NetIO>>(ios, threads, FLAGS_party);
    db_name = (FLAGS_party == ALICE) ? alice_db : bob_db;

}

void ZkBaseTest::TearDown() {

    ios[0]->io->flush();
    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    ASSERT_FALSE(cheat);

    for(int i = 0; i < threads; ++i) {
        delete ios[i]->io;
        delete ios[i];
    }


}
