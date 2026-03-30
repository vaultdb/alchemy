#include <string>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include "util/emp_manager/sh2pc_manager.h"
#include "util/utilities.h"
#include "util/system_configuration.h"
#include "opt/expression_cost_model.h"

using namespace emp;
using namespace std;
using namespace std::chrono;
using namespace vaultdb;

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <party> <port> <alice_host>" << std::endl;
        return -1;
    }

    int party = atoi(argv[1]);
    int port = atoi(argv[2]);
    string host = argv[3];

    // === Setup SH2PCManager ===
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    SH2PCManager *manager = new SH2PCManager(party == ALICE ? "" : host, party, port);
    conf.emp_manager_ = manager;

    cout << "Running Integer Add + Compare benchmark using VaultDB SH2PCManager\n";

    // === Run benchmark ===
    int num_iterations = 200'500'000;

    // Manual cost estimation from ExpressionCostModel
    QueryFieldDesc field_a(0, "a", "dummy_table", FieldType::SECURE_INT, 0);
    field_a.setSize(32);  // 32-bit integer
    size_t gates_per_op = field_a.size();  // 32
    size_t gate_count = gates_per_op * num_iterations;

    int and_before = manager->andGateCount();
    auto time_start = high_resolution_clock::now();

    Integer a(32, 123, ALICE);
    Integer b(32, 124, BOB);

    for (int i = 0; i < num_iterations; ++i) {
        Integer c = a & b;
    }

    auto time_end = high_resolution_clock::now();
    int and_after = manager->andGateCount();
    auto elapsed_ms = duration_cast<milliseconds>(time_end - time_start).count();
    double r_g = static_cast<double>(elapsed_ms) / gate_count;  // runtime per estimated gate

    // === Snapshot once at the end ===
    size_t mem = Utilities::residentMemoryUtilization(false);
    uint64_t cpu = Utilities::getCPUTime();
    auto net = Utilities::getNetworkIO();
    size_t mem_bw = Utilities::getMemoryBandwidth();
    auto disk_io = Utilities::getDiskIOUtilization();

    // === Output only final resource usage ===
    cout << "AND gates count: " << gate_count << " (32 per a & b)" << endl;
    cout << "Observed AND gates used: " << (and_after - and_before) << endl;
    cout << "Runtime (ms): " << elapsed_ms << endl;
    cout << "Runtime per estimated gate (r_g): " << r_g << " ms/gate" << endl;
    cout << "Resident memory (bytes): " << mem << endl;
    cout << "CPU time (clock ticks): " << cpu << endl;

    // Safe network reporting (interface might differ)
    if (net.count("eth0_rx") && net.count("eth0_tx")) {
        cout << "Network RX (bytes): " << net["eth0_rx"] << endl;
        cout << "Network TX (bytes): " << net["eth0_tx"] << endl;
    } else {
        cerr << "Warning: Network stats not available for interface 'eth0'.\n";
    }

    cout << "Memory Bandwidth (MB/s): " << mem_bw << endl;

    // Disk I/O stats (7 fields expected)
    if (disk_io.size() >= 7) {
        cout << "Disk I/O:\n";
        cout << "  rchar (bytes): " << disk_io[0] << endl;
        cout << "  wchar (bytes): " << disk_io[1] << endl;
        cout << "  syscr (reads): " << disk_io[2] << endl;
        cout << "  syscw (writes): " << disk_io[3] << endl;
        cout << "  read_bytes: " << disk_io[4] << endl;
        cout << "  write_bytes: " << disk_io[5] << endl;
        cout << "  cancelled_write_bytes: " << disk_io[6] << endl;
    } else {
        cerr << "Warning: Disk I/O stats not available or incomplete.\n";
    }

    delete manager;
    return 0;
}
