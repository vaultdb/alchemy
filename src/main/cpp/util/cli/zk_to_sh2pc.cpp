#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>
#include <emp-tool/emp-tool.h>

#include "parser/plan_parser.h"
#include "util/emp_manager/sh2pc_manager.h"
#include "util/utilities.h"
#include "util/zk_to_sh2pc/shared_block.h"

#if __has_include("emp-sh2pc/emp-sh2pc.h")

using namespace std;
using namespace vaultdb;

Integer toInt(const block & in, int & my_party, const int & src_party) {
    int64_t *vals = (int64_t *) &in; // might be able to knock this down to a single pass
    int64_t l_val = (my_party == src_party || (src_party == PUBLIC && my_party == ALICE)) ? vals[0] : 0;
    int64_t h_val = (my_party == src_party ||  (src_party == PUBLIC && my_party == ALICE)) ? vals[1] : 0;
    Integer l_int(64,l_val, src_party);
    Integer h_int(64, h_val, src_party);
    Integer res(128, 0, PUBLIC);
    memcpy(res.bits.data(), l_int.bits.data(), l_int.bits.size() * sizeof(Bit));
    memcpy(res.bits.data() + l_int.bits.size(), h_int.bits.data(), h_int.bits.size() * sizeof(Bit));
    return res;
}

block revealToBlock(Integer & src, const int & dst_party) {
    assert(src.bits.size() == 128);
    Integer lhs, rhs;
    int block_bit_cnt = src.bits.size();
    lhs.bits.resize(block_bit_cnt/2);
    rhs.bits.resize(block_bit_cnt/2);
    memcpy(lhs.bits.data(), src.bits.data(), lhs.bits.size() * sizeof(Bit));
    memcpy(rhs.bits.data(), src.bits.data() + lhs.bits.size(), rhs.bits.size() * sizeof(Bit));

    auto b_lhs = lhs.reveal<int64_t>(dst_party);
    auto b_rhs = rhs.reveal<int64_t>(dst_party);

    // high, low bit order
    return emp::makeBlock(b_rhs, b_lhs);

}

int main(int argc, char **argv) {
    // takes in commitments and sends info needed to convert them to secret shares
    // e.g., writing from Alice-ZK to Alice-SH2PC
    // usage: zk_to_sh2pc <party> <alice host> <sh2pc port> <bridge ctrl port>
    // e.g., ./zk_to_sh2pc 1 54324 54330 127.0.0.1

    // ports:
    // SH A <--> B: sh2pc port
    // ZK A --> SH2PC A: ctrl port
    // ZK B --> SH2PC B: ctrl port+1

    if(argc < 5) {
        std::cout << "usage: zk_to_sh2pc <party> <alice host> <sh2pc port> <bridge ctrl port>  " << std::endl;
        exit(-1);
    }
    int party = atoi(argv[1]);
    string host = argv[4];
    int sh2pc_port = atoi(argv[2]);
    int bridge_ctrl_port = atoi(argv[3]);



    bridge_ctrl_port += (party == ALICE ? 0 : 1);
    // new NetIO(party == ALICE ? nullptr    : gen_host.c_str(),port+i)
    cout << "Connecting bridge on port " << bridge_ctrl_port << endl;
    auto bridge_io = new NetIO(host.c_str(), bridge_ctrl_port);
    cout << "Bridge connected.\n";

    SystemConfiguration & conf = SystemConfiguration::getInstance();
    conf.emp_mode_ = EmpMode::SH2PC;
    conf.setStorageModel(StorageModel::COLUMN_STORE);
    auto manager = new SH2PCManager(party == ALICE ? "" : host, party, sh2pc_port);
    conf.emp_manager_ = manager;


    block k;
    bridge_io->recv_data(&k, sizeof(block));
    bridge_io->flush();

    block zero = zero_block;
    block delta = zero;

    bool x = 0;
    block nonce = zero;
    Integer delta_i, nonce_i, k_i;
    Integer zero_i(128, 0L, PUBLIC);
    block to_share = zero_block;
    if(party == ALICE) {
        x = getLSB(k);
        nonce = Utilities::generateRandomBlock();
        int8_t * nonce_bytes = (int8_t *) &nonce;
        int8_t *k_bytes = (int8_t *) &k;
        cout << "Alice received key " << k << " with nonce " << nonce << ".\n";
        for (int i = 0; i < sizeof(block); ++i) {
            k_bytes[i] = nonce_bytes[i] ^ k_bytes[i]; // xor with nonce
        }
        cout << "Appending " << k << " to alice's hash.\n";
    }
    else { // bob
      // one-time send
        bridge_io->recv_data(&delta, sizeof(block));
        bridge_io->flush();
        to_share = k;
        cout << "Bob received delta " << delta << endl;

    }
    cout << "Secret sharing\n";

    delta_i = toInt(delta, party, BOB);
    nonce_i = toInt(nonce, party, ALICE);
    k_i = toInt(to_share, party, BOB);
    Bit x_s(x, ALICE);

    cout << "Getting commitment\n";

    Integer choice = emp::If(x_s, delta_i, zero_i);
    k_i ^= choice; // xor with choice
    k_i ^= nonce_i; // xor with nonce
    cout << "Revealing to block\n";
    auto k2 = revealToBlock(k_i, BOB);

    if(party == BOB) {
        cout << "Appending " << k2 << " to bob's digest.\n";
    }

    cout << "Finished test!\n";
    delete bridge_io;
}
#endif
