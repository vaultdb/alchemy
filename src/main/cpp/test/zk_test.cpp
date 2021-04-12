#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/predicate.h>
#include <query_table/secure_tuple.h>
#include <emp-zk/emp-zk.h>
#include <test/support/ZkBaseTest.h>

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class ZkTest : public ZkBaseTest {

};


// based on example.cpp in emp-zk
TEST_F(ZkTest, example) {
    Integer a(32, 3, ALICE);
    Integer b(32, 2, ALICE);
    cout << (a-b).reveal<uint32_t>(PUBLIC)<<endl;
    // cheat check in base class

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


