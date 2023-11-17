#include "emp-sh2pc/semihonest.h"
#include<cstring>

using namespace std;
using namespace emp;

int max_name_length_ = 10;

bool *bytesToBool(int8_t *bytes, const int & byte_cnt) {
    bool *ret = new bool[byte_cnt * 8];
    bool *writePos = ret;

    for(int i = 0; i < byte_cnt; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}

Integer secretShareString(const string & src, const int & len, const int & sending_party, const int & my_party) {

    int bit_cnt = len * 8;
    emp::Integer secret_shared(bit_cnt, 0L, sending_party);

    if(my_party == sending_party) {
        string input = src;

        // need to reverse string for emp::Integer comparison (>, <=, etc.)
        std::reverse(input.begin(), input.end());
        bool *b = bytesToBool((int8_t *) input.c_str(), len);

        emp::ProtocolExecution::prot_exec->feed((block *) secret_shared.bits.data(), sending_party, b, bit_cnt);
        delete [] b;
    }
    else {
        bool *b = new bool[bit_cnt];
        memset(b, 0, bit_cnt);
        emp::ProtocolExecution::prot_exec->feed((block *) secret_shared.bits.data(), sending_party, b, bit_cnt);
        delete [] b;
    }

    return secret_shared;


}


signed char boolsToByte(const bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}


string revealString(Integer & src, const int & party) {

        auto bit_cnt = src.size();
        auto byte_cnt = bit_cnt / 8;
        assert(bit_cnt % 8 == 0);


        bool *bools = new bool[bit_cnt];
        ProtocolExecution::prot_exec->reveal(bools, party, (block *) src.bits.data(), bit_cnt);
        bool *bool_cursor = bools;

        string dst_str(byte_cnt, ' ');
        char *dst = dst_str.data();

        for(int i = byte_cnt - 1; i >= 0; --i) {
            dst[i] = boolsToByte(bool_cursor);
            bool_cursor += 8;
        }

        delete[] bools;
        return dst_str;

}

// usage: ./entry_comparison <party 1 || 2> <port> <name> <age>

int main(int argc, char **argv) {
    int port, party;
    if(argc < 5) {
        cout << "Usage: ./entry_comparison <party 1 || 2> <port> <name> <age>\n";
        return 1;
    }

    party = atoi(argv[1]);
    port = atoi(argv[2]);

    NetIO *io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);
    setup_semi_honest(io, party);


    string name(argv[3]);
    assert(name.size() <= max_name_length_);

    int age = atoi(argv[4]);
    cout << "Inputting " << name << ", " << age << '\n\n';

    // names are padded to be the same length
    while(name.size() < max_name_length_) {
        name += (char) 0;
    }

    Integer a_age(32, party == ALICE ? age : 0, ALICE);
    Integer b_age(32, party == ALICE ? 0 : age, BOB);

    Integer a_name = secretShareString(party == ALICE ? name : "", max_name_length_, ALICE, party);
    Integer b_name = secretShareString(party == ALICE ? "" : name, max_name_length_, BOB, party);

    Bit eq = (a_name == b_name);
    if(eq.reveal(PUBLIC)) cout << "Names are equal\n\n";
    else cout << "Names are not equal\n\n";

    Bit lt = (a_age < b_age);

    if(lt.reveal(PUBLIC)) cout << "Alice is younger than Bob\n\n";
    else cout << "Bob is younger than Alice\n\n";

    // to reveal name
    string a_name_revealed = revealString(a_name, PUBLIC);
    string b_name_revealed = revealString(b_name, PUBLIC);

    while(a_name_revealed[a_name_revealed.size()-1] == (char) 0) {
        a_name_revealed.pop_back();
    }

    while(b_name_revealed[b_name_revealed.size()-1] == (char) 0) {
        b_name_revealed.pop_back();
    }

    cout << "A's name: " << a_name_revealed << ", B's name: " << b_name_revealed << "\n";

    // to reveal age
    int a_age_revealed = a_age.reveal<int>(PUBLIC);
    int b_age_revealed = b_age.reveal<int>(PUBLIC);
    cout << "A's age: " << a_age_revealed << ", B's age: " << b_age_revealed << "\n";

    finalize_semi_honest();

}