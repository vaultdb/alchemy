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


class ZkFilterPredicateClass : public Predicate<emp::Bit> {

    SecureField line_no;
public:
    ~ZkFilterPredicateClass() {}
    ZkFilterPredicateClass(int32_t to_match) {
        emp::Integer val(32, to_match);
        // encrypting here so we don't have to secret share it for every comparison
        line_no = SecureField(FieldType::SECURE_INT, val);

    }

    // filtering for l_linenumber = 1
    emp::Bit predicateCall(const SecureTuple & tuple) const override {
        const SecureField f =  tuple.getField(1);
        return (f == line_no);
    }

};



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
        delete ios[i]->io;
        delete ios[i];
    }

}


shared_ptr<SecureTable> ZkFilterTest::secret_share_input() const {
    std::string db_name = FLAGS_party == emp::ALICE ? alice_db : bob_db;
    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);

    // only alice can secret share in ZK land
    if(FLAGS_party == emp::ALICE) {
        SqlInput read_db(db_name, sql, false, order_by);
        shared_ptr<PlainTable> input = read_db.run();
        return PlainTable::secret_share_send_table(input, FLAGS_party);
    }
    else if(FLAGS_party == emp::BOB){ // BOB
        // not input, just grabbing schema
        SqlInput read_db(db_name, sql, false);
        shared_ptr<PlainTable> input = read_db.run();
        QuerySchema schema = *input->getSchema();
        return PlainTable ::secret_share_recv_table(tuple_cnt, schema, order_by, emp::ALICE);
    }

    throw std::invalid_argument("Not a valid party for secret sharing!");

}

TEST_F(ZkFilterTest, test_table_scan) {

    // smoke test for basic infrastructure
    std::shared_ptr<SecureTable> shared = secret_share_input();

    std::unique_ptr<PlainTable> revealed = shared->reveal(emp::PUBLIC);
    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, sql, false, 2);



    ASSERT_EQ(*expected, *revealed);
    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();

    ASSERT_FALSE(cheat);

}




// Testing for selecting l_linenumber=1

TEST_F(ZkFilterTest, test_filter) {
    std::string db_name =  FLAGS_party == emp::ALICE ? alice_db : bob_db;

    std::string expected_result_sql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";
    std::shared_ptr<SecureTable> shared = secret_share_input();
    std::shared_ptr<Predicate<emp::Bit> > aPredicate(new ZkFilterPredicateClass(1));  // secret share the constant (1) just once
    Filter<emp::Bit> filter(shared, aPredicate);  // deletion handled by shared_ptr

    std::shared_ptr<SecureTable> result = filter.run();
    std::unique_ptr<PlainTable> revealed = result->reveal(emp::PUBLIC);

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, expected_result_sql, true, 2);

    ASSERT_EQ(*expected,  *revealed);

    bool cheat = finalize_zk_bool<BoolIO<NetIO>>();

    ASSERT_FALSE(cheat);



}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


