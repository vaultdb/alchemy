#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>
#include <emp-tool/emp-tool.h>
#include "parser/plan_parser.h"
#include "test/enrich_test.h"
#include "util/emp_manager/zk_manager.h"
#include "util/utilities.h"


#if __has_include("emp-zk/emp-zk.h")

using namespace std;
using namespace vaultdb;

int main(int argc, char **argv) {
    // takes in commitments and sends info needed to convert them to secret shares
    // e.g., writing from Alice-ZK to Alice-SH2PC
    // usage: zk_to_sh2pc_in <party> <alice host> <zk port> <bridge ctrl port>
    // e.g., ./zk_to_sh2pc_in 1 54321 54330 127.0.0.1

    // ports:
    // ZK A <--> B: ZK port
    // ZK A --> SH2PC A: ctrl port
    // ZK B --> SH2PC B: ctrl port+1

    if(argc < 5) {
        std::cout << "usage: zk_to_sh2pc_in <party>  <zk port> <bridge ctrl port> <alice host>  " << std::endl;
        exit(-1);
    }
    int party = atoi(argv[1]);
    string host = argv[4];
    int zk_port = atoi(argv[2]);
    int bridge_ctrl_port = atoi(argv[3]);

    bridge_ctrl_port += (party == ALICE ? 0 : 1);
    cout << "Connecting bridge on port " << bridge_ctrl_port << endl;
    // new NetIO(party == ALICE ? nullptr    : gen_host.c_str(),port+i)
    auto bridge_io = new NetIO(nullptr, bridge_ctrl_port);
    cout << "Bridge connected.\n";

    SystemConfiguration & conf = SystemConfiguration::getInstance();
    conf.emp_mode_ = EmpMode::ZK;
    conf.setStorageModel(StorageModel::COLUMN_STORE);
    ZKManager *manager = new ZKManager(party == ALICE ? "" : host, party, zk_port);
    conf.emp_manager_ = manager;

    bool input =  0; //  (party  == ALICE); // alice inputs 1, bob inputs 0
    Bit test(input, ALICE);

    auto k = test.bit;
    cout << "Sending " << k << '\n';
    bridge_io->send_data(&k, sizeof(block));
    bridge_io->flush();


    manager->flush();

    if (party == BOB) {
        auto verifier = ((ZKVerifier<emp::BoolIO<emp::NetIO>> *)CircuitExecution::circ_exec);
        // one-time send
        block delta = verifier->ostriple->delta;
        cout << "Sending delta: " << delta  << '\n';
        bridge_io->send_data(&delta, sizeof(block));
        bridge_io->flush();
        cout << "Finished sending delta" << '\n';
    }

    cout << "Finished test!\n";
    delete bridge_io;
}
#endif
