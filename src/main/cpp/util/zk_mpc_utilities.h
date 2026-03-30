#ifndef ZK_MPC_UTILITIES_H
#define ZK_MPC_UTILITIES_H

#include <emp-tool/emp-tool.h>
#include <util/emp_manager/emp_manager.h>
#include <util/emp_manager/zk_manager.h>
#include <util/emp_manager/sh2pc_manager.h>
#include <util/emp_manager/outsourced_mpc_manager.h>
#include <common/defs.h>
#include <vector>
#include <string>
#include <cstring>

using namespace emp;
using namespace vaultdb;

// Convert a block to a vector of bools
inline vector<bool> block_to_bits(const block& b) {
    std::vector<bool> bits;
    const uint64_t* w = reinterpret_cast<const uint64_t*>(&b); // w[0]=low, w[1]=high
    for (int i = 0; i < 64; ++i) bits.push_back((w[0] >> i) & 1ULL);
    for (int i = 0; i < 64; ++i) bits.push_back((w[1] >> i) & 1ULL);
    return bits;
}

// Convert a vector of bools to a block
inline block bits_to_block(const std::vector<bool>& bits) {
    uint64_t lo = 0, hi = 0;
    const size_t n = bits.size();
    const size_t n_lo = n > 64 ? 64 : n;
    for (size_t i = 0; i < n_lo; ++i)
        if (bits[i]) lo |= (1ULL << i);
    for (size_t i = 64; i < n && i < 128; ++i)
        if (bits[i]) hi |= (1ULL << (i - 64));
    return makeBlock(hi, lo);
}

#if __has_include("emp-zk/emp-zk.h") || __has_include("emp-sh2pc/emp-sh2pc.h")

#include <emp-zk/emp-zk.h>
#include <emp-sh2pc/emp-sh2pc.h>

class ZkToMPCUtilities {
    public:
        EmpManager *emp_manager_ = nullptr;

        std::string zk_host_;
        std::vector<int> zk_ports_;
        int party_;
        bool is_zk_ = false;
        bool is_sh_2pc_ = false;

         // m_x = k_x ^ (x & delta)
        block m_x_ = zero_block;
        block k_x_ = zero_block;
        block delta_ = zero_block;

        Hash hash_;

        bool is_authenticated = false;

        ZkToMPCUtilities(EmpManager *emp_manager, const std::string &zk_host, const std::vector<int> &zk_ports, int party, bool is_zk, bool is_sh_2pc) : emp_manager_(emp_manager), zk_host_(zk_host), zk_ports_(zk_ports), party_(party), is_zk_(is_zk), is_sh_2pc_(is_sh_2pc) {}
        
        Bit zkToSH2PCOneBit(Bit & bit) {
            assert(is_zk_ || is_sh_2pc_);

            if(is_zk_) {
                if(party_ == ALICE) {
                    bool true_bit = getLSB(bit.bit);
                    printf("true_bit: %d\n", true_bit);

                    // update m_x
                    m_x_ =  bit.bit;
                    cout << "m_x: " << m_x_ << endl;

                    NetIO *zk_io = new NetIO(nullptr, zk_ports_[0]);

                    // send m_x to sh2pc Alice
                    zk_io->send_block(&m_x_, 1);
                    zk_io->flush();
                    cout << "Sent m_x to sh2pc Alice" << endl;
                    
                    zk_io->send_bool(&true_bit, 1);
                    zk_io->flush();
                    cout << "Sent true bit to sh2pc Alice" << endl;

                    delete zk_io;
                }
                else {
                    assert(party_ == BOB);

                    ZKVerifier<BoolIO<NetIO>> * zk_verifier = static_cast<ZKVerifier<BoolIO<NetIO>>*>(emp::ProtocolExecution::prot_exec);

                    // update k_x
                    k_x_ = bit.bit;
                    cout << "k_x: " << k_x_ << endl;

                    // update delta
                    delta_ = zk_verifier->ostriple->delta;
                    cout << "delta: " << delta_ << endl;

                    NetIO *zk_io = new NetIO(nullptr, zk_ports_[1]);

                    // send k_x to sh2pc Bob
                    zk_io->send_block(&k_x_, 1);
                    zk_io->flush();
                    cout << "Sent k_x to sh2pc Bob" << endl;
                    
                    // send delta to sh2pc Bob
                    zk_io->send_block(&delta_, 1);
                    zk_io->flush();
                    cout << "Sent delta to sh2pc Bob" << endl;

                    delete zk_verifier;
                    delete zk_io;
                }

                return bit;
            }

            if(is_sh_2pc_) {
                Bit sh2pc_bit;

                if(party_ == ALICE) {
                    NetIO *zk_io = new NetIO(zk_host_.c_str(), zk_ports_[0]);

                    // receive m_x from sh2pc Alice
                    zk_io->recv_block(&m_x_, 1);
                    zk_io->flush();
                    cout << "Received m_x from sh2pc Alice" << endl;

                    // receive true bit from sh2pc Alice
                    bool true_bit;
                    zk_io->recv_bool(&true_bit, 1);
                    zk_io->flush();
                    cout << "Received true bit from sh2pc Alice" << endl;

                    delete zk_io;

                    sh2pc_bit = Bit(true_bit, ALICE);
                    cout << "sh2pc_bit: " << sh2pc_bit.reveal<bool>(PUBLIC) << endl;

                    // Alice generate an nonce r - 128-bit block
                    cout << "Generating an nonce r - 128-bit block." << endl;
                    PRG prg;
                    vector<bool> r_plaintext_bits;
                    vector<Bit> r_bits;
                    for(int i = 0; i < 128; ++i) {
                        bool r;
                        prg.random_bool(&r, 1);
                        r_plaintext_bits.push_back(r);
                        r_bits.push_back(Bit(r, ALICE));
                    }
                    cout << "Sent r\n";
                    block r_block = bits_to_block(r_plaintext_bits);
                    cout << "r_block: " << r_block << endl;

                    // Alice receive the shares of k_x
                    vector<Bit> k_x_bits;
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits.push_back(Bit(0, BOB));
                    }
                    cout << "Secret shared k_x\n";

                    // Alice receive the shares of delta
                    vector<Bit> delta_bits;
                    for(int i = 0; i < 128; ++i) {
                        delta_bits.push_back(Bit(0, BOB));
                    }
                    cout << "Secret shared delta\n";

                    // compute [k_x] = [k_x] ^ [x] & [delta]
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits[i] = k_x_bits[i] ^ (sh2pc_bit & delta_bits[i]);
                    }
                    cout << "Computed [k_x] = [k_x] ^ [x] & [delta]\n";

                    // compute [k_x] = [k_x] ^ [r]
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits[i] = k_x_bits[i] ^ r_bits[i];
                    }
                    cout << "Computed [k_x] = [k_x] ^ [r]\n";

                    // reveal [k_x]
                    vector<bool> result_k_x_plaintext_bits;
                    for(int i = 0; i < 128; ++i) {
                        result_k_x_plaintext_bits.push_back(k_x_bits[i].reveal<bool>(PUBLIC));
                    }
                    cout << "Revealed [k_x]\n";
                    block rhs = bits_to_block(result_k_x_plaintext_bits);
                    cout << "rhs: " << rhs << endl;

                    // m_x ^ r
                    block lhs = m_x_ ^ r_block;
                    cout << "lhs: " << lhs << endl;

                    char digest[emp::Hash::DIGEST_SIZE];
                    hash_.put_block(&lhs, 1);
                    hash_.digest(digest);

                    ((ZKManager *) emp_manager_)->ios_[0]->io->send_data(digest, emp::Hash::DIGEST_SIZE);
                    ((ZKManager *) emp_manager_)->ios_[0]->io->flush();
                    cout << "Sent digest" << endl;
                } 
                else {
                    assert(party_ == BOB);

                    NetIO *zk_io = new NetIO(zk_host_.c_str(), zk_ports_[1]);

                    // receive k_x from sh2pc Bob
                    zk_io->recv_block(&k_x_, 1);
                    zk_io->flush();
                    cout << "Received k_x from sh2pc Bob" << endl;
                    
                    // receive delta from sh2pc Bob
                    zk_io->recv_block(&delta_, 1);
                    zk_io->flush();
                    cout << "Received delta from sh2pc Bob" << endl;

                    delete zk_io;

                    sh2pc_bit = Bit(0, ALICE);
                    cout << "sh2pc_bit: " << sh2pc_bit.reveal<bool>(PUBLIC) << endl;

                    // Bob receive the nonce r
                    cout << "Receiving an nonce r - 128-bit block." << endl;
                    vector<Bit> r_bits;
                    for(int i = 0; i < 128; ++i) {
                        r_bits.push_back(Bit(0, ALICE));
                    }
                    cout << "Received r\n";

                    // Bob secret share k_x
                    vector<bool> k_x_plaintext_bits = block_to_bits(k_x_);
                    vector<Bit> k_x_bits;
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits.push_back(Bit(k_x_plaintext_bits[i], BOB));
                    }
                    cout << "Secret shared k_x\n";

                    // Bob secret share delta
                    vector<bool> delta_plaintext_bits = block_to_bits(delta_);
                    vector<Bit> delta_bits;
                    for(int i = 0; i < 128; ++i) {
                        delta_bits.push_back(Bit(delta_plaintext_bits[i], BOB));
                    }
                    cout << "Secret shared delta\n";

                    // compute [k_x] = [k_x] ^ [x] & [delta]
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits[i] = k_x_bits[i] ^ (sh2pc_bit & delta_bits[i]);
                    }
                    cout << "Computed [k_x] = [k_x] ^ [x] & [delta]\n";

                    // compute [k_x] = [k_x] ^ [r]
                    for(int i = 0; i < 128; ++i) {
                        k_x_bits[i] = k_x_bits[i] ^ r_bits[i];
                    }
                    cout << "Computed [k_x] = [k_x] ^ [r]\n";

                    // reveal [k_x]
                    vector<bool> result_k_x_plaintext_bits;
                    for(int i = 0; i < 128; ++i) {
                        result_k_x_plaintext_bits.push_back(k_x_bits[i].reveal<bool>(PUBLIC));
                    }
                    cout << "Revealed [k_x]\n";
                    block rhs = bits_to_block(result_k_x_plaintext_bits);
                    cout << "rhs: " << rhs << endl;

                    // verify with hash
                    hash_.put_block(&rhs, 1);

                    char digest[emp::Hash::DIGEST_SIZE];
                    hash_.digest(digest);

                    char digest2[emp::Hash::DIGEST_SIZE];
                    ((ZKManager *) emp_manager_)->ios_[0]->io->recv_data(digest2, emp::Hash::DIGEST_SIZE);
                    ((ZKManager *) emp_manager_)->ios_[0]->io->flush();
                    cout << "Received digest" << endl;

                    is_authenticated = memcmp(digest, digest2, emp::Hash::DIGEST_SIZE) == 0;
                    cout << (is_authenticated ? "Hashes are equal" : "hashes are not equal") << endl;

                    assert(is_authenticated);
                }

                return sh2pc_bit;
            }

            return bit;
        }

        Bit ZKToMPCOneBit(Bit & bit) {
            assert(is_zk_ && !is_sh_2pc_);

            NetIO *zk_io = new NetIO(nullptr, zk_ports_[0]);

            if(party_ == ALICE) {
                bool true_bit = getLSB(bit.bit);
                printf("true_bit: %d\n", true_bit);

                // update m_x
                m_x_ =  bit.bit;
                cout << "m_x: " << m_x_ << endl;

                // send m_x to MPC Evaluator
                zk_io->send_block(&m_x_, 1);
                zk_io->flush();
                cout << "Sent m_x to MPC Evaluator" << endl;
                
                // send true bit to MPC Evaluator
                zk_io->send_bool(&true_bit, 1);
                zk_io->flush();
                cout << "Sent true bit to MPC Evaluator" << endl;
            }
            else {
                assert(party_ == BOB);

                ZKVerifier<BoolIO<NetIO>> * zk_verifier = static_cast<ZKVerifier<BoolIO<NetIO>>*>(emp::ProtocolExecution::prot_exec);

                // update k_x
                k_x_ = bit.bit;
                cout << "k_x: " << k_x_ << endl;

                // update delta
                delta_ = zk_verifier->ostriple->delta;
                cout << "delta: " << delta_ << endl;

                // send k_x to MPC Garbler
                zk_io->send_block(&k_x_, 1);
                zk_io->flush();
                cout << "Sent k_x to MPC Garbler" << endl;
                
                // send delta to MPC Garbler
                zk_io->send_block(&delta_, 1);
                zk_io->flush();
                cout << "Sent delta to MPC Garbler" << endl;

                delete zk_verifier;
            }

            delete zk_io;

            return bit;
        }

        ~ZkToMPCUtilities() {
            if (emp_manager_ != nullptr) {
                delete emp_manager_;
            }
        }
};


#elif __has_include("emp-rescu/emp-rescu.h")

class ZkToMPCUtilities {
    public:
        EmpManager *emp_manager_ = nullptr;

        std::string zk_host_;
        std::vector<int> zk_ports_;
        int party_;
        bool is_zk_ = false;
        bool is_sh_2pc_ = false;

        block m_x_ = zero_block;
        block k_x_ = zero_block;
        block delta_ = zero_block;

        Hash hash_;

        bool is_authenticated = false;

        ZkToMPCUtilities(EmpManager *emp_manager, const std::string &zk_host, const std::vector<int> &zk_ports, int party, bool is_zk, bool is_sh_2pc) : emp_manager_(emp_manager), zk_host_(zk_host), zk_ports_(zk_ports), party_(party), is_zk_(is_zk), is_sh_2pc_(is_sh_2pc) {}

        Bit zkToSH2PCOneBit(Bit & bit) {
            return bit;
        }

        Bit ZKToMPCOneBit(Bit & bit) {
            assert(!is_zk_ && !is_sh_2pc_);

            Bit ompc_bit;
            Bit final_check_bit; // = 1 if lhs (evaluator) hash digest == rhs hash digest, computed by all parties in MPC

            if(party_ == ALICE) {
                cout << "Evaluator is secret sharing the authenticated bit to MPC." << endl;
                vector<NetIO *> zk_ios;

                vector<block> m_x_blocks;
                bool true_bit;

                for(int i = 0; i < zk_ports_.size(); ++i) {
                    zk_ios.push_back(new NetIO(zk_host_.c_str(), zk_ports_[i]));
                }

                for(int i = 0; i < zk_ios.size(); ++i) {
                    zk_ios[i]->recv_block(&m_x_blocks[i], 1);
                    zk_ios[i]->flush();
                    cout << "Received m_x from ZK" << endl;
                    cout << "m_x: " << m_x_blocks[i] << endl;

                    if(i == 0) {
                        zk_ios[i]->recv_bool(&true_bit, 1);
                        zk_ios[i]->flush();
                        cout << "Received true bit from ZK" << endl;
                        cout << "true_bit: " << true_bit << endl;
                    }
                }

                // Evaluator secret share x
                ompc_bit = Bit(true_bit, ALICE);
                cout << "Secret shared x\n";

                // Evaluator generate an nonce r - 128-bit block
                cout << "Generating an nonce r - 128-bit block." << endl;
                PRG prg;
                vector<bool> r_plaintext_bits;
                vector<Bit> r_bits;
                for(int i = 0; i < 128; ++i) {
                    bool r;
                    prg.random_bool(&r, 1);
                    r_plaintext_bits.push_back(r);
                    r_bits.push_back(Bit(r, ALICE));
                }
                cout << "Sent r\n";
                block r_block = bits_to_block(r_plaintext_bits);
                cout << "r_block: " << r_block << endl;

                // Evaluator receives the shares of k_x and delta
                vector<vector<Bit>> k_x_bits_vec;
                vector<vector<Bit>> delta_bits_vec;

                for(int party_idx = BOB; party_idx < N + 1; ++party_idx) {
                    vector<Bit> others_k_x_bits;
                    for(int i = 0; i < 128; ++i) {
                        others_k_x_bits.push_back(Bit(0, party_idx));
                    }
                    k_x_bits_vec.push_back(others_k_x_bits);
                    cout << "Received k_x from party " << party_idx << "\n";

                    vector<Bit> others_delta_bits;
                    for(int i = 0; i < 128; ++i) {
                        others_delta_bits.push_back(Bit(0, party_idx));
                    }
                    delta_bits_vec.push_back(others_delta_bits);
                    cout << "Received delta from party " << party_idx << "\n";
                }

                // Evaluator computes m_x_b ^ m_x_c ^ r in plaintext
                // Get the hash of lhs
                // Secret share the hash to other parties
                block lhs = r_block;
                for(int i = 0; i < N - 1; ++i) {
                    lhs = lhs ^ m_x_blocks[i];
                }
                cout << "lhs: " << lhs << endl;

                char lhs_digest[emp::Hash::DIGEST_SIZE];
                hash_.put_block(&lhs, 1);
                hash_.digest(lhs_digest);

                // secret sharing evaluator digest to emp::Integer
                emp::Integer evaluator_digest_int(emp::Hash::DIGEST_SIZE*8, 0, ALICE);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE; ++i) {
                    for(int j = 0; j < 8; ++j) {
                        evaluator_digest_int.bits[i*8 + j] = Bit((lhs_digest[i] >> j) & 1u, ALICE);
                    }
                }
                cout << "Secret shared evaluator digest\n";

                // All parties compute [rhs] = [k_x_b] ^ ([delta_b] & [x]) ^ [k_x_c] ^ ([delta_c] & [x]) ^ [r]
                // Open rhs to TP
                // TP compute the hash of rhs and secret share it to other parties
                vector<Bit> rhs_bits;
                for(int i = 0; i < 128; ++i) {
                    Bit rhs_bit = k_x_bits_vec[0][i] ^ (delta_bits_vec[0][i] & ompc_bit) ^ k_x_bits_vec[1][i] ^ (delta_bits_vec[1][i] & ompc_bit) ^ r_bits[i];
                    rhs_bits.push_back(rhs_bit);
                }
                cout << "Computed secret shared rhs\n";

                // reveal [rhs] to TP
                for(int i = 0; i < 128; ++i) {
                    rhs_bits[i].reveal(TP);
                }
                cout << "Revealed [rhs] to TP\n";

                // secret sharing rhs digest to emp::Integer by TP
                emp::Integer rhs_digest_int(emp::Hash::DIGEST_SIZE*8, 0, TP);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE * 8; ++i) {
                    rhs_digest_int.bits[i] = Bit(0, TP);
                }
                cout << "Received shares of rhs digest from TP\n";

                // All parties compute final_check_bit = [evaluator_digest_int] == [rhs_digest_int]
                final_check_bit = evaluator_digest_int == rhs_digest_int;
                cout << "Computed final_check_bit\n";

                for(int i = 0; i < zk_ios.size(); ++i) {
                    delete zk_ios[i];
                }

            }
            else if(party_ != TP) {
                cout << "Garblers are receiving the authenticated bit from MPC." << endl;

                NetIO *zk_io = new NetIO(zk_host_.c_str(), zk_ports_[0]);
                zk_io->recv_block(&k_x_, 1);
                zk_io->flush();
                cout << "Received k_x from MPC" << endl;
                cout << "k_x: " << k_x_ << endl;

                zk_io->recv_block(&delta_, 1);
                zk_io->flush();
                cout << "Received delta from MPC" << endl;
                cout << "delta: " << delta_ << endl;

                delete zk_io;

                // Garblers receive the share x
                ompc_bit = Bit(0, ALICE);
                cout << "Received shares of x\n";

                // Garblers receive the nonce r
                cout << "Receiving an nonce r - 128-bit block." << endl;
                vector<Bit> r_bits;
                for(int i = 0; i < 128; ++i) {
                    r_bits.push_back(Bit(0, ALICE));
                }
                cout << "Received r\n";

                // Garbler individually secret share k_x and delta
                vector<vector<Bit>> k_x_bits_vec;
                vector<vector<Bit>> delta_bits_vec;

                for(int party_idx = BOB; party_idx < N + 1; ++party_idx) {
                    if(party_ == party_idx) {
                        vector<bool> k_x_plaintext_bits = block_to_bits(k_x_);
                        vector<Bit> k_x_bits;
                        for(int i = 0; i < 128; ++i) {
                            k_x_bits.push_back(Bit(k_x_plaintext_bits[i], party_idx));
                        }
                        k_x_bits_vec.push_back(k_x_bits);
                        cout << "Secret shared k_x from party " << party_idx << "\n";

                        vector<bool> delta_plaintext_bits = block_to_bits(delta_);
                        vector<Bit> delta_bits;
                        for(int i = 0; i < 128; ++i) {
                            delta_bits.push_back(Bit(delta_plaintext_bits[i], party_idx));
                        }
                        delta_bits_vec.push_back(delta_bits);
                        cout << "Secret shared delta from party " << party_idx << "\n";
                    }
                    else {
                        vector<Bit> others_k_x_bits;
                        for(int i = 0; i < 128; ++i) {
                            others_k_x_bits.push_back(Bit(0, party_idx));
                        }
                        k_x_bits_vec.push_back(others_k_x_bits);
                        cout << "Received k_x from party " << party_idx << "\n";

                        vector<Bit> others_delta_bits;
                        for(int i = 0; i < 128; ++i) {
                            others_delta_bits.push_back(Bit(0, party_idx));
                        }
                        delta_bits_vec.push_back(others_delta_bits);
                        cout << "Received delta from party " << party_idx << "\n";
                    }
                }

                //  Receive the shares of evaluator digest
                emp::Integer evaluator_digest_int(emp::Hash::DIGEST_SIZE*8, 0, ALICE);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE * 8; ++i) {
                    evaluator_digest_int.bits[i] = Bit(0, ALICE);
                }
                cout << "Received shares of evaluator digest\n";

                // All parties compute [rhs] = [k_x_b] ^ ([delta_b] & [x]) ^ [k_x_c] ^ ([delta_c] & [x]) ^ [r]
                // Open rhs to TP
                // TP compute the hash of rhs and secret share it to other parties
                vector<Bit> rhs_bits;
                for(int i = 0; i < 128; ++i) {
                    //Bit rhs_bit = k_x_bits_vec[0][i] ^ (delta_bits_vec[0][i] & ompc_bit) ^ k_x_bits_vec[1][i] ^ (delta_bits_vec[1][i] & ompc_bit) ^ r_bits[i];
                    Bit rhs_bit = r_bits[i];
                    for(int j = 0; j < N - 1; ++j) {
                        rhs_bit = rhs_bit ^ k_x_bits_vec[j][i] ^ (delta_bits_vec[j][i] & ompc_bit);
                    }
                    rhs_bits.push_back(rhs_bit);
                }
                cout << "Computed secret shared rhs\n";

                // reveal [rhs] to TP
                for(int i = 0; i < 128; ++i) {
                    rhs_bits[i].reveal(TP);
                }
                cout << "Revealed [rhs] to TP\n";

                // secret sharing rhs digest to emp::Integer by TP
                emp::Integer rhs_digest_int(emp::Hash::DIGEST_SIZE*8, 0, TP);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE * 8; ++i) {
                    rhs_digest_int.bits[i] = Bit(0, TP);
                }
                cout << "Received shares of rhs digest from TP\n";

                // All parties compute final_check_bit = [evaluator_digest_int] == [rhs_digest_int]
                final_check_bit = evaluator_digest_int == rhs_digest_int;
                cout << "Computed final_check_bit\n";
            }
            else {
                assert(party_ == TP);

                // TP receive the share x
                ompc_bit = Bit(0, ALICE);
                cout << "Received shares of x\n";

                // TP receive the nonce r
                cout << "Receiving an nonce r - 128-bit block." << endl;
                vector<Bit> r_bits;
                for(int i = 0; i < 128; ++i) {
                    r_bits.push_back(Bit(0, ALICE));
                }
                cout << "Received r\n";

                // TP receives the shares of k_x and delta
                vector<vector<Bit>> k_x_bits_vec;
                vector<vector<Bit>> delta_bits_vec;

                for(int party_idx = BOB; party_idx < N + 1; ++party_idx) {
                    vector<Bit> others_k_x_bits;
                    for(int i = 0; i < 128; ++i) {
                        others_k_x_bits.push_back(Bit(0, party_idx));
                    }
                    k_x_bits_vec.push_back(others_k_x_bits);
                    cout << "Received k_x from party " << party_idx << "\n";

                    vector<Bit> others_delta_bits;
                    for(int i = 0; i < 128; ++i) {
                        others_delta_bits.push_back(Bit(0, party_idx));
                    }
                    delta_bits_vec.push_back(others_delta_bits);
                    cout << "Received delta from party " << party_idx << "\n";
                }

                //  Receive the shares of evaluator digest
                emp::Integer evaluator_digest_int(emp::Hash::DIGEST_SIZE*8, 0, ALICE);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE * 8; ++i) {
                    evaluator_digest_int.bits[i] = Bit(0, ALICE);
                }
                cout << "Received shares of evaluator digest\n";

                // All parties compute [rhs] = [k_x_b] ^ ([delta_b] & [x]) ^ [k_x_c] ^ ([delta_c] & [x]) ^ [r]
                // Open rhs to TP
                // TP compute the hash of rhs and secret share it to other parties
                vector<Bit> rhs_bits;
                for(int i = 0; i < 128; ++i) {
                    Bit rhs_bit = k_x_bits_vec[0][i] ^ (delta_bits_vec[0][i] & ompc_bit) ^ k_x_bits_vec[1][i] ^ (delta_bits_vec[1][i] & ompc_bit) ^ r_bits[i];
                    rhs_bits.push_back(rhs_bit);
                }
                cout << "Computed secret shared rhs\n";

                // reveal [rhs] to TP
                vector<bool> result_rhs_bits;
                for(int i = 0; i < 128; ++i) {
                    result_rhs_bits.push_back(rhs_bits[i].reveal(TP));
                }
                cout << "Revealed [rhs] to TP\n";
                block rhs = bits_to_block(result_rhs_bits);
                cout << "rhs: " << rhs << endl;

                // compute the hash of rhs
                char rhs_digest[emp::Hash::DIGEST_SIZE];
                hash.put_block(&rhs, 1);
                hash.digest(rhs_digest);

                // secret sharing rhs digest to emp::Integer by TP
                emp::Integer rhs_digest_int(emp::Hash::DIGEST_SIZE*8, 0, TP);
                for(int i = 0; i < emp::Hash::DIGEST_SIZE; ++i) {
                    for(int j = 0; j < 8; ++j) {
                        rhs_digest_int.bits[i*8 + j] = Bit((rhs_digest[i] >> j) & 1u, TP);
                    }
                }
                cout << "Secret shared rhs digest by TP\n";

                // All parties compute final_check_bit = [evaluator_digest_int] == [rhs_digest_int]
                final_check_bit = evaluator_digest_int == rhs_digest_int;
                cout << "Computed final_check_bit\n";
            }

            bool is_equal = final_check_bit.reveal(PUBLIC);
            cout << (is_equal ? "Hashes are equal and finish the authentication" : "Hashes are not equal and reject the bit") << endl;
            assert(is_equal);

            return ompc_bit;
        }

        ~ZkToMPCUtilities() {
            if (emp_manager_ != nullptr) {
                delete emp_manager_;
            }
        }
};

#else

class ZkToOMPCUtilities {
};

#endif

#endif // ZK_MPC_UTILITIES_H