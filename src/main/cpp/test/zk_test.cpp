#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/predicate.h>
#include <query_table/secure_tuple.h>
#include <emp-zk/emp-zk.h>


using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class ZkFilterTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    std::string alice_db = "tpch_alice";
    std::string bob_db = "tpch_bob";
    static const int threads = 1;
    static const int tuple_cnt = 10;
    std::string sql;

    BoolIO<NetIO>* ios[threads];

    shared_ptr<SecureTable> secret_share_input() const;

};

void ZkFilterTest::SetUp() {

    BoolIO<NetIO>* ios[threads];
    for(int i = 0; i < threads; ++i)
        ios[i] = new BoolIO<NetIO>(new NetIO(FLAGS_party == ALICE?nullptr: FLAGS_alice_host.c_str(),FLAGS_port+i), FLAGS_party==ALICE);

    sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT " + std::to_string(tuple_cnt);

    setup_zk_bool<BoolIO<NetIO>>(ios, threads, FLAGS_party);

}

void ZkFilterTest::TearDown() {
    for(int i = 0; i < threads; ++i) {
        if(ios[i]->io != nullptr) delete ios[i]->io;
        if(ios[i] != nullptr) delete ios[i];
    }

}

// based on example.cpp in emp-zk
TEST_F(ZkFilterTest, example) {
    Integer a(32, 3, ALICE);
    Integer b(32, 2, ALICE);
    cout << (a-b).reveal<uint32_t>(PUBLIC)<<endl;

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();
    std::cout<< "cheat = " << cheat << std::endl;



    ASSERT_FALSE(cheat);
    std::cout << "Completed successfully!" << std::endl;
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


