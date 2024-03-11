#include "catalyst.h"



int main(int argc, char **argv) {
    // usage: ./bin/catalyst <json config file>
    if(argc < 2) {
        std::cout << "usage: ./bin/catalyst <json config file>" << std::endl;
        exit(-1);
    }

    Catalyst catalyst(argv[1]);


}