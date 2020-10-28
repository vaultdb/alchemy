#include <emp-tool/emp-tool.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include "utils.h"

using namespace  emp;
using namespace  std;

int main(int argc, char **argv) {

    int party, port;

    parse_party_and_port(argv, &party, &port);

    NetIO *netio =  new NetIO(party == emp::ALICE ? nullptr : "127.0.0.1", port);
    setup_semi_honest(netio, party,  1024*16);

    int32_t shares[4];
    string filename = party == emp::ALICE ? "alice.enc" : "bob.enc";
    ifstream inputFile(filename, ios::in | ios::binary);
    inputFile.read((char *) shares, 16);
    inputFile.close();

    Integer aliceShares[4];
    Integer bobShares[4];
    Integer recoveredShares[4];

    for(int i = 0; i < 4; ++i) {
        aliceShares[i] = (party == ALICE) ? Integer(32, shares[i], ALICE) : Integer(32, 0, ALICE);
    }

    for(int i = 0; i < 4; ++i) {
        bobShares[i] = (party == BOB) ? Integer(32, shares[i], BOB) : Integer(32, 0, BOB);
    }

    for(int i = 0; i < 4; ++i) {
        recoveredShares[i] = aliceShares[i] ^ bobShares[i];
    }


    // expected:  {12345, 23456, 34567, 45678}
    assert(recoveredShares[0].reveal<int32_t>() == 12345);
    assert(recoveredShares[1].reveal<int32_t>() == 23456);
    assert(recoveredShares[2].reveal<int32_t>() == 34567);
    assert(recoveredShares[3].reveal<int32_t>() == 45678);

    finalize_semi_honest();
    delete netio;
    cout << "Success! party=" << party <<  endl;
}
