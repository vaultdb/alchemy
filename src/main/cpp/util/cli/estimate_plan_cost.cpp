#include <string>
#include <unistd.h>
#include <iostream>
#include <util/emp_manager/outsourced_mpc_manager.h>

#include "util/utilities.h"
#include "parser/plan_parser.h"
#include "opt/operator_cost_model.h"
#include "util/emp_manager/sh2pc_manager.h"


using namespace std;
using namespace vaultdb;

int main(int argc, char **argv) {
    // usage: parse_plan <src directory> <query name>
    // target path is relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/estimate_plan_cost tpch_unioned_150 0.00065 conf/plans/mpc-q5.json
    // 0.00065 ms is estimated ms/gate for m7a multi-host

    if (argc < 4) {
        std::cout << "usage: estimate_plan_cost  <db name> <time per gate in ms> <src json>" << std::endl;
        exit(-1);
    }

    string db_name = argv[1];
    double ms_per_gate = std::strtod(argv[2], nullptr);
    string filename = argv[3];
    string plan_json = Utilities::getCurrentWorkingDirectory() + "/" + filename;
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(db_name);

    SystemConfiguration &s = SystemConfiguration::getInstance();
    s.initialize(db_name, md, StorageModel::COLUMN_STORE);

    s.setStorageModel(StorageModel::COLUMN_STORE);
#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")
    auto manager = new SH2PCManager();
    s.emp_manager_ = manager;
#else
    auto manager = new OutsourcedMpcManager();
    string data_path = Utilities::getCurrentWorkingDirectory() + "/wires/" + db_name;
    string temp_data_path = Utilities::getCurrentWorkingDirectory() + "/wires/temp";
    s.setStorageModel(StorageModel::PACKED_COLUMN_STORE);
    s.initializeWirePacking(data_path, temp_data_path);
    s.emp_manager_ = manager;

#endif


    cout << "Parsing " << filename << " on " << db_name << '\n';

    PlanParser<Bit> plan_reader(db_name, plan_json, -1, true);
    SecureOperator *root = plan_reader.getRoot();
    cout << "Plan: " << root->printTree() << endl;

    size_t estimated_gates = OperatorCostModel::planCost(root);
    cout << "Estimated cost for " << filename << " : " << estimated_gates << " gates" << endl;
    size_t estimated_memory = OperatorCostModel::planPeakMemory(root);
    cout << "Estimated memory for " << estimated_memory << " bytes" << endl;
    double_t runtime_secs = (estimated_gates * ms_per_gate)/1000.0;
    double_t runtime_hours = runtime_secs / 3600.0;

    cout << "Estimated runtime: " << runtime_secs << " seconds (" << runtime_hours << " hours).\n";
    // delete manager; this is covered in the SystemConfiguration destructor
}
