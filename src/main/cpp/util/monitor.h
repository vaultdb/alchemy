#ifndef _VAULTDB_UTIL_MONITOR_H_
#define _VAULTDB_UTIL_MONITOR_H_

#include <thread>
#include <atomic>
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include "util/utilities.h"

using namespace std;

namespace vaultdb {
    class Monitor {

    public:
        // Static members to share the log file path and ensure thread-safe logging.
        static std::string log_file_path;
        static std::mutex log_mutex;

        std::atomic<bool> keep_running_;
        std::atomic<bool> first_report_;
        std::atomic<unsigned long long> last_cumulative_rchar_;
        std::atomic<unsigned long long> last_cumulative_wchar_;
        std::atomic<unsigned long long> last_cumulative_syscr_;
        std::atomic<unsigned long long> last_cumulative_syscw_;
        std::atomic<unsigned long long> last_cumulative_read_bytes_;
        std::atomic<unsigned long long> last_cumulative_write_bytes_;
        std::atomic<unsigned long long> last_cumulative_cancelled_write_bytes_;
        std::atomic<uint64_t> last_cpu_time_;
        std::atomic<unsigned long long> last_cumulative_rx_bytes_;
        std::atomic<unsigned long long> last_cumulative_tx_bytes_;
        int interval_ = 5; // 5 seconds between measurements
        std::string working_network_interface_ = "eno1"; // Check it on Linux with "netstat -tunap"

        Monitor() {
            // initialize all state
            keep_running_.store(true);
            first_report_.store(true);
            last_cumulative_rchar_.store(-1);
            last_cumulative_wchar_.store(-1);
            last_cumulative_syscr_.store(-1);
            last_cumulative_syscw_.store(-1);
            last_cumulative_read_bytes_.store(-1);
            last_cumulative_write_bytes_.store(-1);
            last_cumulative_cancelled_write_bytes_.store(-1);
            last_cpu_time_.store(-1);
            last_cumulative_rx_bytes_.store(-1);
            last_cumulative_tx_bytes_.store(-1);
        }

        void monitor(string dst_file, string working_network_interface) {
            // Prefer the static log file path if it has been set.
            std::string file_to_use;
            {
                std::lock_guard<std::mutex> guard(log_mutex);
                file_to_use = (!log_file_path.empty()) ? log_file_path : dst_file;
            }

            std::ofstream file_out;
            std::ostream *os;
            if (file_to_use.empty()) {
                os = &std::cout;
            } else {
                // Open the file in append mode
                file_out.open(file_to_use.c_str(), std::ios::out | std::ios::app);
                if (!file_out) {
                    std::cerr << "Error: Could not open log file: " << file_to_use << std::endl;
                    os = &std::cout;
                } else {
                    os = &file_out;
                }
            }

            working_network_interface_ = working_network_interface;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (keep_running_.load()) {
                string output_str;
                output_str += "Report stats in each " + to_string(interval_) + " seconds:\n";

                // Memory
                size_t peak_mem = Utilities::checkMemoryUtilization(false);
                size_t resident_mem = Utilities::residentMemoryUtilization(false);
                size_t swap = Utilities::checkSwapUtilization(false);
                output_str += "Current Memory Footprint: " + to_string(resident_mem) + " bytes, Current Swap Footprint: " +
                              to_string(swap) + " bytes, Peak Memory Footprint: " + to_string(peak_mem) + " bytes\n";

                // Disk I/O
                std::vector<unsigned long long> io_values = Utilities::getDiskIOUtilization();
                string extra_output_str = "\ncumulative rchar: " + to_string(io_values[0]) +
                                          ", cumulative wchar: " + to_string(io_values[1]) +
                                          ", cumulative syscr: " + to_string(io_values[2]) +
                                          ", cumulative syscw: " + to_string(io_values[3]) +
                                          ", cumulative read_bytes: " + to_string(io_values[4]) +
                                          ", cumulative write_bytes: " + to_string(io_values[5]) +
                                          ", cumulative cancelled_write_bytes: " + to_string(io_values[6]) + "\n";

                // CPU
                uint64_t current_cpu_time = Utilities::getCPUTime();

                // Network I/O
                std::map<std::string, unsigned long long> network_stats = Utilities::getNetworkIO();

                if (first_report_.load()) {
                    // Initialize previous values for the first report
                    last_cumulative_rchar_ = io_values[0];
                    last_cumulative_wchar_ = io_values[1];
                    last_cumulative_syscr_ = io_values[2];
                    last_cumulative_syscw_ = io_values[3];
                    last_cumulative_read_bytes_ = io_values[4];
                    last_cumulative_write_bytes_ = io_values[5];
                    last_cumulative_cancelled_write_bytes_ = io_values[6];
                    last_cpu_time_ = current_cpu_time;
                    last_cumulative_rx_bytes_ = network_stats[working_network_interface_ + "_rx"];
                    last_cumulative_tx_bytes_ = network_stats[working_network_interface_ + "_tx"];
                } else {
                    // Disk I/O differences
                    unsigned long long current_cumulative_rchar = io_values[0];
                    unsigned long long current_cumulative_wchar = io_values[1];
                    unsigned long long current_cumulative_syscr = io_values[2];
                    unsigned long long current_cumulative_syscw = io_values[3];
                    unsigned long long current_cumulative_read_bytes = io_values[4];
                    unsigned long long current_cumulative_write_bytes = io_values[5];
                    unsigned long long current_cumulative_cancelled_write_bytes = io_values[6];

                    unsigned long long rchar = current_cumulative_rchar - last_cumulative_rchar_.load();
                    unsigned long long wchar = current_cumulative_wchar - last_cumulative_wchar_.load();
                    unsigned long long syscr = current_cumulative_syscr - last_cumulative_syscr_.load();
                    unsigned long long syscw = current_cumulative_syscw - last_cumulative_syscw_.load();
                    unsigned long long read_bytes = current_cumulative_read_bytes - last_cumulative_read_bytes_.load();
                    unsigned long long write_bytes = current_cumulative_write_bytes - last_cumulative_write_bytes_.load();
                    unsigned long long cancelled_write_bytes = current_cumulative_cancelled_write_bytes - last_cumulative_cancelled_write_bytes_.load();

                    last_cumulative_rchar_ = current_cumulative_rchar;
                    last_cumulative_wchar_ = current_cumulative_wchar;
                    last_cumulative_syscr_ = current_cumulative_syscr;
                    last_cumulative_syscw_ = current_cumulative_syscw;
                    last_cumulative_read_bytes_ = current_cumulative_read_bytes;
                    last_cumulative_write_bytes_ = current_cumulative_write_bytes;
                    last_cumulative_cancelled_write_bytes_ = current_cumulative_cancelled_write_bytes;

                    output_str += "rchar: " + to_string(rchar) +
                                  ", wchar: " + to_string(wchar) +
                                  ", syscr: " + to_string(syscr) +
                                  ", syscw: " + to_string(syscw) +
                                  ", read_bytes: " + to_string(read_bytes) +
                                  ", write_bytes: " + to_string(write_bytes) +
                                  ", cancelled_write_bytes: " + to_string(cancelled_write_bytes) + "\n";

                    // CPU usage
                    unsigned long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
                    double cpu_usage = Utilities::getCPUUsage(last_cpu_time_.load(), current_cpu_time, interval_);
                    double normalized_cpu_usage = cpu_usage / num_cores;
                    output_str += "CPU Usage (Normalized across " + to_string(num_cores) +
                                  " cores): " + to_string(normalized_cpu_usage) + "%\n";
                    if (normalized_cpu_usage > 100) {
                        throw std::runtime_error("Normalized cpu usage cannot be over 100%");
                    }
                    last_cpu_time_ = current_cpu_time;

                    // Network I/O differences
                    unsigned long long current_rx_bytes = network_stats[working_network_interface_ + "_rx"];
                    unsigned long long current_tx_bytes = network_stats[working_network_interface_ + "_tx"];
                    unsigned long long rx_diff = current_rx_bytes - last_cumulative_rx_bytes_.load();
                    unsigned long long tx_diff = current_tx_bytes - last_cumulative_tx_bytes_.load();
                    last_cumulative_rx_bytes_ = current_rx_bytes;
                    last_cumulative_tx_bytes_ = current_tx_bytes;

                    unsigned long long rx_speed = rx_diff / interval_;
                    unsigned long long tx_speed = tx_diff / interval_;

                    output_str += "Network RX: " + to_string(rx_diff) + " bytes, Network TX: " + to_string(tx_diff) +
                                  " bytes, RX Speed: " + to_string(rx_speed) + " bytes/s, TX Speed: " +
                                  to_string(tx_speed) + " bytes/s, Total Network Speed: " +
                                  to_string(rx_speed + tx_speed) + " bytes/s\n";
                }

                // Write extra_output_str and output_str in a thread-safe way using the static mutex.
                {
                    std::lock_guard<std::mutex> lock(log_mutex);
                    *os << extra_output_str << endl;
                    if (first_report_.load()) {
                        first_report_ = false;
                    } else {
                        *os << output_str << endl;
                    }
                }

                std::this_thread::sleep_for(std::chrono::seconds(interval_));
            }

            if (file_out.is_open()) {
                file_out.close();
            }
        }
    };

    // Definitions for static members
    std::string Monitor::log_file_path = "";
    std::mutex Monitor::log_mutex;
}
#endif