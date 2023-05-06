#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

using namespace std;


int main(int argc, char **argv) {
    // usage: parse_plan <src directory> <query name>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/parse_plan tpch_unioned conf/plans/ q1

    if (argc < 4) {
        std::cout << "usage: parse_plan  <db name>  <src directory> <query name>" << std::endl;
        exit(-1);
    }

    string db_name = argv[1];
    string src_path = argv[2];
    string test_name = argv[3];

    SecureOperator root = PlanParser<bool>::parsePlan()
    PlanParser<bool> plan_reader(db_name, test_name, limit_);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();
    std::string observed_plan = root->printTree();
}