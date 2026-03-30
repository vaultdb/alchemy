#ifndef _VAULTDB_UTIL_MONITOR_CSV_H_
#define _VAULTDB_UTIL_MONITOR_CSV_H_

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
    class MonitorCsv {
    public:
        std::atomic<bool> keep_running_;

        MonitorCsv() {
            keep_running_.store(true);
        }

        void monitor(int interval_secs, string dst_file, string network_device) {
            std::ofstream file_out;
            std::ostream *os;
            if (dst_file.empty()) {
                os = &std::cout;
            } else {
                file_out.open(dst_file.c_str(), std::ios::out | std::ios::app);
                if (!file_out) {
                    std::cerr << "Error: Could not open log file: " << dst_file << std::endl;
                    os = &std::cout;
                } else {
                    os = &file_out;
                }
            }

            auto network_rx_key = network_device + "_rx";
            auto network_tx_key = network_device + "_tx";

            std::this_thread::sleep_for(std::chrono::seconds(1));
            size_t timestep = 0;

            auto last_io_values = Utilities::getDiskIOUtilization();
            size_t last_cpu_time = Utilities::getCPUTime();
            auto network_stats = Utilities::getNetworkIO();
            auto last_network_tx_bytes = network_stats[network_tx_key];
            auto last_network_rx_bytes = network_stats[network_rx_key];

            auto last_swap_io = Utilities::checkSwapIOUtilization();

            // Updated CSV header with new metrics
            *os << "timestamp (secs),active memory (bytes),swap (bytes),peak memory (bytes),rchar (bytes),"
                   "wchar (bytes),syscr (bytes),syscw (bytes),read_bytes,write_bytes,cancelled_write_bytes,"
                   "cpu time (clock ticks),network recvd (Bps),network sent (Bps),"
                   "memory bandwidth (MB/s),L3 cache misses,CPU stall cycles,IPC,swap in pages,swap in bytes,swap in speed (Bps),swap out pages,swap out bytes,swap out speed (Bps)\n";

            while (keep_running_.load()) {
                // Memory
                size_t peak_mem = Utilities::checkMemoryUtilization(false);
                size_t resident_mem = Utilities::residentMemoryUtilization(false);
                size_t swap = Utilities::checkSwapUtilization(false);
                *os << timestep << "," << resident_mem << "," << swap << "," << peak_mem;

                // Disk I/O
                auto io_values = Utilities::getDiskIOUtilization();
                for (int i = 0; i < 7; ++i) {
                    *os <<  ","  <<  (io_values[i] - last_io_values[i]);
                }
                last_io_values = io_values;

                // CPU - clock ticks
                uint64_t current_cpu_time = Utilities::getCPUTime();
                *os << "," << (current_cpu_time - last_cpu_time);
                last_cpu_time = current_cpu_time;

                // Network I/O
                auto network_stats = Utilities::getNetworkIO();
                *os << "," << (network_stats[network_rx_key] - last_network_rx_bytes) / ((double) interval_secs) << ","
                    << (network_stats[network_tx_key] - last_network_tx_bytes) / ((double) interval_secs);

                last_network_rx_bytes = network_stats[network_rx_key];
                last_network_tx_bytes = network_stats[network_tx_key];

                // **NEW METRICS**
                size_t memory_bandwidth = Utilities::getMemoryBandwidth();
                size_t l3_cache_misses = Utilities::getL3CacheMisses();
                size_t cpu_stall_cycles = Utilities::getCPUStallCycles();
                double ipc = Utilities::getIPC();

                *os << "," << memory_bandwidth << "," << l3_cache_misses << "," << cpu_stall_cycles << "," << ipc;

                // Swap I/O
                auto swap_io = Utilities::checkSwapIOUtilization();
                *os << "," << swap_io[0] - last_swap_io[0] << "," << swap_io[1] - last_swap_io[1] << "," << (swap_io[1] - last_swap_io[1]) / ((double) interval_secs) << ","
                    << swap_io[2] - last_swap_io[2] << "," << swap_io[3] - last_swap_io[3] << "," << (swap_io[3] - last_swap_io[3]) / ((double) interval_secs) << "\n";
                last_swap_io = swap_io;

                std::this_thread::sleep_for(std::chrono::seconds(interval_secs));
                timestep += interval_secs;
                os->flush();
            }

            if (file_out.is_open()) {
                file_out.close();
            }
        }
    };

}
#endif
