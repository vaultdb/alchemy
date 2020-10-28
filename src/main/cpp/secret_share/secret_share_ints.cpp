#include "prg.h"
#include <fstream>


// simple mockup to demo secret sharing facility
// subtract out dependencies on emp toolkit for data sharing (not compute) hosts
void writeFile(std::string fileName, const char *contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if(!outFile.is_open()) {
        throw "Could not write output file " + fileName;
    }
    outFile.write(contents, 16);
    outFile.close();
}

int main(int argc, char **argv) {

    emp::PRG prg; // initializes with a random seed
    int32_t data[4] = {12345, 23456, 34567, 45678};

    int32_t r[4]; // random values
    int32_t rx[4]; // r XOR data

    prg.random_data(&r, 4);

    for(int i = 0; i < 4; ++i) {
        rx[i] = r[i] ^ data[i];
    }

    writeFile("alice.enc", (char *) &r);
    writeFile("bob.enc", (char *) &rx);


}
