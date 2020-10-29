#include "EmpBaseTest.h"


//NetIO *EmpBaseTest::netio = nullptr;

void EmpBaseTest::SetUp()  {

    std::cout << "Connecting to " << FLAGS_alice_host << " on port " << FLAGS_port << " as " << FLAGS_party << std::endl;
    netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party,  1024*16);
    // increment the port for each new test
    ++FLAGS_port;

}

void EmpBaseTest::TearDown() {
    std::cout << "Tearing down test suite!" << std::endl;
        netio->flush();
        delete netio;
        //sleep(2);
}
