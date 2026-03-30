#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>
#include "parser/plan_parser.h"
#include "util/utilities.h"


using namespace std;
using namespace vaultdb;

#define REVEAL_RESULT 1

int main(int argc, char **argv) {
    // usage: run_plain_query <db_name> <query plan json>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./run_plain_query  tpch_unioned_150 conf/plans/mpc-q5.json

    if(argc < 3) {
        std::cout << "usage: run_plain_query <db_name> <query plan json>" << std::endl;
        exit(-1);
    }

    string db_name = argv[1];
    string plan_file = argv[2];
    srand (time(nullptr));

    plan_file = Utilities::getCurrentWorkingDirectory() + "/" + plan_file;
    cout << "Parsing query plan from " << plan_file << endl;
    PlanParser<bool> plan_reader(db_name, plan_file, 0, true);
    PlainOperator *root = plan_reader.getRoot();
    cout << "Running plan: \n" << root->printTree() << endl;

//    SystemConfiguration & conf = SystemConfiguration::getInstance();
//    conf.emp_mode_ = EmpMode::SH2PC;
//    conf.setStorageModel(StorageModel::COLUMN_STORE);
//    SH2PCManager *manager = new SH2PCManager(party == ALICE ? "" : host, party, port);
//    conf.emp_manager_ = manager;

    auto start = std::chrono::high_resolution_clock::now();
    PlainTable *res = root->run();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Query execution time: " << elapsed.count() << "s" << std::endl;

    if(REVEAL_RESULT) {
        PlainTable *observed = res->reveal();
        cout << "Query answer: " << *observed << endl;
        delete observed;
    }




}