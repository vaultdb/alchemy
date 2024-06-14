#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>
#include "emp_manager/sh2pc_manager.h"
#include "utilities.h"
#include "parser/plan_parser.h"


using namespace std;
using namespace vaultdb;

#define REVEAL_RESULT 0

int main(int argc, char **argv) {
    // usage: run_query <party> <db_name for this party> <query plan json>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./run_query 1 tpch_alice_600 conf/plans/

    if(argc < 5) {
        std::cout << "usage: run_query <party> <port> <alice_host> <db_name for this party> <query plan json>" << std::endl;
        exit(-1);
    }
    int party = atoi(argv[1]);
    int port = atoi(argv[2]);
    string host = argv[3];
    string db_name = argv[4];
    string plan_file = argv[5];
    srand (time(nullptr));

    plan_file = Utilities::getCurrentWorkingDirectory() + "/" + plan_file;
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    conf.emp_mode_ = EmpMode::SH2PC;
    conf.setStorageModel(StorageModel::COLUMN_STORE);
    SH2PCManager *manager = new SH2PCManager(party == ALICE ? "" : host, party, port);
    conf.emp_manager_ = manager;

    cout << "Parsing query plan from " << plan_file << endl;
    PlanParser<Bit> plan_reader(db_name, plan_file, 0, true);
    SecureOperator *root = plan_reader.getRoot();
    cout << "Running plan: \n" << root->printTree() << endl;

    auto start = std::chrono::high_resolution_clock::now();
    SecureTable *res = root->run();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Query execution time: " << elapsed.count() << "s" << std::endl;

    if(REVEAL_RESULT) {
        PlainTable *observed = res->reveal();
        cout << "Query answer: " << *observed << endl;
        delete observed;
    }




}