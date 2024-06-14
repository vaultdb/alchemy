#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unistd.h>
#include <iostream>
#include "util/utilities.h"
#include "parser/plan_parser.h"


using namespace std;
using namespace vaultdb;

int main(int argc, char **argv) {
    // usage: parse_plan <src directory> <query name>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/parse_plan tpch_unioned_150 conf/plans/mpc-q5.json

    if (argc < 3) {
        std::cout << "usage: parse_plan  <db name>  <src json>" << std::endl;
        exit(-1);
    }

    string db_name = argv[1];
    string plan_json = Utilities::getCurrentWorkingDirectory() + "/" + argv[2];


    cout << "Parsing " << plan_json << endl;

    PlanParser<bool> plan_reader(db_name, plan_json, -1, true);
    PlainOperator *root = plan_reader.getRoot();

    cout << "Parsed plan: " << root->printTree() << endl;

}
