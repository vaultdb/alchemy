#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <test/zk/zk_base_test.h>

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54328, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class ZkUnitTest : public ZkTest {

};


// based on example.cpp in emp-zk
TEST_F(ZkUnitTest, example) {
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


