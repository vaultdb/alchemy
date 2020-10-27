//
// Created by Jennie Rogers on 10/27/20.
//

//
// Created by Jennie Rogers on 8/2/20.
//
// set up to be self-contained

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class EmpTestMultipleStarts : public ::testing::Test {
protected:
     void SetUp() override {

         std::cout << "Connecting to " << FLAGS_alice_host << " on port " << FLAGS_port << " as " << FLAGS_party << std::endl;
         netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
         emp::setup_semi_honest(netio, FLAGS_party,  1024*16);
     }

     void TearDown() override {
         netio->flush();
         delete CircuitExecution::circ_exec;
         delete ProtocolExecution::prot_exec;
         delete netio;
         sleep(2);

     }
    emp::NetIO *netio;

     static emp::Integer encryptVarchar(std::string input, size_t stringBitCount, const int & myParty, const int & dstParty);
     static std::string revealVarchar(const emp::Integer & input);

    static int8_t *boolsToBytes(const bool *const src, const uint32_t &bitCount);
    static bool *bytesToBool(int8_t *bytes, int byteCount);
    static signed char boolsToByte(const bool *src);

    void passPlainInt();
};



TEST_F(EmpTestMultipleStarts, emp_pass_plain_int) {
  passPlainInt();
}
//  verify emp configuration for int32s from both ALICE and BOB
TEST_F(EmpTestMultipleStarts, emp_test_int) {


    // test encrypting an int from ALICE
    passPlainInt();
    int32_t inputValue =  FLAGS_party == emp::ALICE ? 5 : 0;
    emp::Integer aliceSecretShared = emp::Integer(32, inputValue,  emp::ALICE);
    //int32_t aliceRevealed = aliceSecretShared.reveal<int32_t>(emp::PUBLIC);
    std::string aliceRevealed = aliceSecretShared.reveal<std::string>(emp::PUBLIC);


    // test encrypting int from BOB
    inputValue =  FLAGS_party == emp::ALICE ? 0 : 4;
    emp::Integer bobSecretShared = emp::Integer(32, inputValue,  emp::BOB);

    std::string bobRevealed = bobSecretShared.reveal<std::string>(emp::PUBLIC);

    std::cout << "Alice revealed: " << aliceRevealed << " expected 5" << std::endl;
    std::cout << "Bob revealed: " << bobRevealed << " expected 4" << std::endl;

    ASSERT_EQ(aliceRevealed, "10100000000000000000000000000000"); // 5
    ASSERT_EQ(bobRevealed, "00100000000000000000000000000000"); // 4
  //  ASSERT_EQ(5, aliceRevealed);
  //  ASSERT_EQ(4, bobRevealed);

}

TEST_F(EmpTestMultipleStarts, emp_test_float) {


    // test encrypting float from ALICE

    passPlainInt();
    float aliceInput =  FLAGS_party == emp::ALICE ? 7.5 : 0;
    emp::Float aliceSecretShared(aliceInput, emp::ALICE);
    float aliceRevealed = aliceSecretShared.reveal<double>(emp::PUBLIC);



    // test encrypting float from BOB
    float bobInput =  FLAGS_party == emp::ALICE ? 0 : 4.5;
    emp::Float bobSecretShared(bobInput,  emp::BOB);
    float bobRevealed = bobSecretShared.reveal<double>(emp::PUBLIC);


    ASSERT_EQ(7.5, aliceRevealed);
    ASSERT_EQ(4.5, bobRevealed);

}


//  verify emp configuration for strings
TEST_F(EmpTestMultipleStarts, emp_test_varchar) {


    // TPC-H strings
    passPlainInt();

    std::string lhsStr = "EGYPT                    ";
    std::string rhsStr = "ARGENTINA                ";

    // encrypt as ALICE
    emp::Integer lhsEncrypted = encryptVarchar(FLAGS_party == emp::ALICE ? lhsStr : std::string(""),
                                                lhsStr.length() * 8, FLAGS_party,
                                                             emp::ALICE);
    // encrypt as BOB
    emp::Integer rhsEncrypted = encryptVarchar(FLAGS_party == emp::ALICE ?  std::string("") : rhsStr,
                                                             rhsStr.length() * 8, FLAGS_party,
                                                             emp::BOB);

    netio->flush();

    std::string aliceRevealed = revealVarchar(lhsEncrypted);
    std::string bobRevealed = revealVarchar(rhsEncrypted);

    ASSERT_EQ(lhsStr, aliceRevealed);
    ASSERT_EQ(rhsStr, bobRevealed);

}

emp::Integer EmpTestMultipleStarts::encryptVarchar(std::string input, size_t stringBitCount, const int & myParty, const int & dstParty) {


    size_t stringByteCount = stringBitCount / 8;

    assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

    while(input.length() != stringByteCount) { // pad it to the right length
        input += " ";
    }

    bool *bools = bytesToBool((int8_t *) input.c_str(), stringByteCount);

    emp::Integer result(stringBitCount, 0L, dstParty);
    if(myParty == dstParty) {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, stringBitCount);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, stringBitCount);
    }


    delete [] bools;


    return result;

}

std::string EmpTestMultipleStarts::revealVarchar(const Integer &encryptedString) {
    long bitCount = encryptedString.size();
    long byteCount = bitCount / 8;


    bool *bools = new bool[bitCount];
    // hackity, hack, hack!
    std::string bitString = encryptedString.reveal<std::string>(emp::PUBLIC);
    std::string::iterator strPos = bitString.begin();
    for(int i =  0; i < bitCount; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    // make it null-terminated
    char *decodedBytes = (char *) boolsToBytes(bools, bitCount);
    char *intermediary = new char[byteCount + 1];
    memcpy(intermediary, decodedBytes, byteCount);
    intermediary[byteCount] = '\0';



    std::string dst(intermediary);

    delete [] decodedBytes;
    delete [] bools;
    delete [] intermediary;

    return dst;
}



int8_t *EmpTestMultipleStarts::boolsToBytes(const bool *const src, const uint32_t &bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    int8_t *result = new int8_t[byteCount];

    bool *cursor = const_cast<bool*>(src);

    for(int i = 0; i < byteCount; ++i) {
        result[i] = boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}


signed char EmpTestMultipleStarts::boolsToByte(const bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}

bool *EmpTestMultipleStarts::bytesToBool(int8_t *bytes, int byteCount) {
    bool *ret = new bool[byteCount * 8];


    bool *writePos = ret;

    for(int i = 0; i < byteCount; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}

void EmpTestMultipleStarts::passPlainInt() {
    int32_t  aliceSize = 32;
    int32_t bobSize = 16;

    if (FLAGS_party == emp::ALICE) {
        netio->send_data(&aliceSize, 4);
        netio->flush();
        netio->recv_data(&bobSize, 4);
        netio->flush();
    } else if (FLAGS_party == emp::BOB) {
        netio->recv_data(&aliceSize, 4);
        netio->flush();
        netio->send_data(&bobSize, 4);
        netio->flush();
    }

    ASSERT_EQ(aliceSize, 32);
    ASSERT_EQ(bobSize, 16);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}