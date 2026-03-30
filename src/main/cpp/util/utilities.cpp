#include "utilities.h"

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <chrono>
#include <boost/stacktrace.hpp>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <common/defs.h>
#include <query_table/query_table.h>
#include <query_table/secure_tuple.h>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif



using namespace std::chrono;
using namespace vaultdb;

// start from $VAULTDB_ROOT/src/main/cpp
std::string Utilities::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    std::string  current_working_dir = std::string(cwd);

    while(!std::filesystem::exists(current_working_dir + "/CMakeLists.txt")) {
        current_working_dir += "/..";
    }

    return current_working_dir;
}



// From Chenkai Li's EMP memory instrumentation

void Utilities::checkMemoryUtilization(const std::string & msg) {
    std::cout << "Checking memory utilization " << msg << std::endl;
    Utilities::checkMemoryUtilization(true);
}

size_t Utilities::checkMemoryUtilization(bool print) {
#if defined(__linux__)
    struct rusage rusage;
    if (!getrusage(RUSAGE_SELF, &rusage)) {
        if(print) {
            size_t current_memory = Utilities::residentMemoryUtilization();
            if(print)
                std::cout << "[Linux]Peak resident set size: " << (size_t) rusage.ru_maxrss * 1024L // kb --> bytes
                          << " bytes, current memory size: " << current_memory << " bytes.\n";
        }
        return (size_t)rusage.ru_maxrss * 1024L;
    }
    else {
        std::cout << "[Linux]Query RSS failed" << std::endl;
        return 0;
    }
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
        if(print)
            std::cout << "[Mac]Peak resident set size: " << (size_t)info.resident_size_max << " bytes, current memory size: " << (size_t)info.resident_size  <<  std::endl;
        return (size_t)info.resident_size_max;
    }
    else {
        std::cout << "[Mac]Query RSS failed" << std::endl;
        return 0;
    }
#endif

}

// from Nadeau's tool:
// https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-runtime-using-c

size_t Utilities::residentMemoryUtilization(bool print) {

#if defined(__linux__)
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    size_t res = (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);

    /* BSD, Linux, and OSX -------------------------------------- */
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage );
    size_t peak_usage =  (size_t)(rusage.ru_maxrss * 1024L);
    if(print)
        std::cout << "[Linux] Peak resident set size: " << peak_usage  << " bytes, current memory size: " <<  res  <<  std::endl;
    return res;
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
        if(print)
            std::cout << "[Mac]Peak resident set size: " << (size_t)info.resident_size_max << " bytes, current memory size: " << (size_t)info.resident_size  <<  std::endl;
        return (size_t)info.resident_size_max;
    }
    else {
        std::cout << "[Mac]Query RSS failed" << std::endl;
        return 0;
    }
#endif
}

size_t Utilities::checkSwapUtilization(bool print) {
#if defined(__linux__)
    std::ifstream status_file("/proc/self/status");
    std::string line;

    if (status_file.is_open()) {
        while (std::getline(status_file, line)) {
            if (line.find("VmSwap:") == 0) {
                std::string swap_kb_str = line.substr(line.find_first_of("0123456789"));
                long swap_kb = std::stol(swap_kb_str);
                long swap_bytes = swap_kb * 1024;
                if(print)
                    std::cout << "Swap space usage: " << swap_bytes << " bytes" << std::endl;
                return swap_bytes;
            }
        }
        status_file.close();
    } else {
        std::cerr << "Unable to open /proc/self/status" << std::endl;
        return -1;
    }
#endif
    return 0;
}

vector<size_t> Utilities::checkSwapIOUtilization() {
#if defined(__linux__)
    std::ifstream vm_status_file("/proc/vmstat");
    std::string line;

    long pageSize = sysconf(_SC_PAGESIZE);

    vector<size_t> swap_io;

    if(vm_status_file.is_open()) {
        while(std::getline(vm_status_file, line)) {
            if(line.find("pswpin") == 0) {
                std::string pswpin_str = line.substr(line.find_first_of("0123456789"));
                long pswpin_pages = std::stol(pswpin_str);
                swap_io.push_back(pswpin_pages);

                size_t pswpin_bytes = pswpin_pages * pageSize;
                swap_io.push_back(pswpin_bytes);
            }

            if(line.find("pswpout") == 0) {
                std::string pswpout_str = line.substr(line.find_first_of("0123456789"));
                long pswpout_pages = std::stol(pswpout_str);
                swap_io.push_back(pswpout_pages);

                size_t pswpout_bytes = pswpout_pages * pageSize;
                swap_io.push_back(pswpout_bytes);
            }
        }
        vm_status_file.close();

        return swap_io;
    }
    else {
        std::cerr << "Unable to open /proc/vmstat" << std::endl;
        return vector<size_t>();
    }

#endif
    return vector<size_t>();
}

size_t Utilities::checkMemoryAndSwapUtilization() {
#if defined(__linux__)
    size_t current_memory = Utilities::residentMemoryUtilization();
    size_t current_swap = Utilities::checkSwapUtilization();
    size_t sum = current_memory + current_swap;
    std::cout << "Current memory + swap utilization: " << sum << " bytes" << std::endl;
    return sum;
#endif
    return 0;
}

void Utilities::checkDiskIOUtilization() {
#if defined(__linux__)
    std::ifstream status_file("/proc/self/io");
    std::string line;

    if (status_file.is_open()) {
        cout << "Disk I/O utilization:" << endl;
        while (std::getline(status_file, line)) {
            cout << line << endl;
        }
        status_file.close();
    } else {
        std::cerr << "Unable to open /proc/self/io" << std::endl;
    }
#endif
}

std::vector<unsigned long long> Utilities::getDiskIOUtilization() {
#if defined(__linux__)
    std::ifstream status_file("/proc/self/io");
    std::vector<unsigned long long> io_values;

    if (status_file.is_open()) {
        std::string line;
        while (std::getline(status_file, line)) {
            std::istringstream iss(line);
            std::string key;
            unsigned long long value;
            if (iss >> key >> value) {
                io_values.push_back(value);
            } else {
                // Handle bad line, perhaps log an error or skip
                throw std::runtime_error("Error occurs during /proc/self/io");
            }
        }
        status_file.close();
    } else {
        throw std::runtime_error("Unable to open /proc/self/io");
    }

    return io_values;
#endif
    // Non-Linux platform, return an empty vector
    return std::vector<unsigned long long>();
}

uint64_t Utilities::getCPUTime() {
#if defined(__linux__)
    std::ifstream stat_file("/proc/self/stat");
    std::string line;
    uint64_t utime = 0, stime = 0;

    if (stat_file.is_open()) {
        std::getline(stat_file, line);
        std::istringstream iss(line);
        std::string token;
        for (int i = 1; i <= 15; ++i) {
            iss >> token;
            if (i == 14) utime = std::stoull(token); // User mode CPU time
            if (i == 15) stime = std::stoull(token); // Kernel mode CPU time
        }
        stat_file.close();

    } else {
        throw std::runtime_error("Unable to open /proc/self/stat");
    }

    return utime + stime; // Total CPU time in clock ticks
#endif
    // Only works for Linux for now
    return 0;
}

// CPU Usage (%) = Delta of CPU Time / (Clock ticks/s * Elapsed Time) * 100
// For multi-core system, we need to normalized it with number of cores to get real cpu usage (%) across the system
double Utilities::getCPUUsage(uint64_t start_cpu_time, uint64_t end_cpu_time, int elapsed_time) {
    static const long clock_ticks_per_second = sysconf(_SC_CLK_TCK); // Clock ticks per second

//    cout << "start cpu time: " << start_cpu_time << endl;
//    cout << "end cpu time: " << end_cpu_time << endl;

    double cpu_usage = (end_cpu_time - start_cpu_time) / static_cast<double>(clock_ticks_per_second) / elapsed_time * 100.0;

    return cpu_usage;
}

std::map<std::string, unsigned long long> Utilities::getNetworkIO() {
    std::ifstream net_dev_file("/proc/self/net/dev");
    std::string line;
    std::map<std::string, unsigned long long> network_stats;

    if (net_dev_file.is_open()) {
        // Skip the first two header lines
        std::getline(net_dev_file, line);
        //cout << line << endl;
        std::getline(net_dev_file, line);
        //cout << line << endl;

        // Parse each line
        while (std::getline(net_dev_file, line)) {
            //cout << line << endl;
            std::istringstream iss(line);
            std::string interface;
            unsigned long long rx_bytes, tx_bytes;

            iss >> interface;

            if (interface.back() == ':') {
                interface.pop_back();
            }

            iss >> rx_bytes;

            for (int i = 0; i < 7; ++i) {
                // Skip the middle values
                uint64_t  dummy;
                iss >> dummy;
            }

            iss >> tx_bytes;

            network_stats[interface + "_rx"] = rx_bytes;
            network_stats[interface + "_tx"] = tx_bytes;

            //cout << "interface: " << interface << ", rx_bytes: " << rx_bytes << ", tx_bytes: " << tx_bytes << endl;
        }
        net_dev_file.close();
    } else {
        throw std::runtime_error("Unable to open /proc/net/dev");
    }

    return network_stats;
}

// **Get Memory Bandwidth (MB/s)**
size_t Utilities::getMemoryBandwidth() {
    std::ifstream file("/sys/devices/system/memory/bandwidth");
    size_t bandwidth = 0;
    if (file.is_open()) {
        file >> bandwidth;
    }
    return bandwidth;
}

// **Get L3 Cache Misses**
size_t Utilities::getL3CacheMisses() {
    std::ifstream file("/sys/devices/cpu/cpu0/cache/index3/misses");
    size_t misses = 0;
    if (file.is_open()) {
        file >> misses;
    }
    return misses;
}

// **Get CPU Stall Cycles**
size_t Utilities::getCPUStallCycles() {
    std::ifstream file("/sys/devices/cpu/cpu0/stall");
    size_t stalls = 0;
    if (file.is_open()) {
        file >> stalls;
    }
    return stalls;
}

// **Get IPC (Instructions Per Cycle)**
double Utilities::getIPC() {
    size_t instructions = 0, cycles = 0;
    std::ifstream file("/proc/cpuinfo");
    std::string line;

    std::ifstream perf_file("/sys/devices/cpu/perf_events");
    while (std::getline(perf_file, line)) {
        if (line.find("instructions") != std::string::npos) {
            instructions = std::stoul(line.substr(line.find(":") + 1));
        } else if (line.find("cycles") != std::string::npos) {
            cycles = std::stoul(line.substr(line.find(":") + 1));
        }
    }

    return (cycles > 0) ? (double)instructions / cycles : 0;
}



std::string Utilities::getStackTrace() {
    std::ostringstream  os;
    os <<  boost::stacktrace::stacktrace();
    return os.str();
}

std::vector<int8_t> Utilities::boolsToBytes( std::string &bitString) {
    int srcBits = bitString.length();
    std::string::iterator strPos = bitString.begin();
    bool *bools = new bool[srcBits];

    for(int i =  0; i < srcBits; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    std::vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, srcBits);
    delete[] bools;
    return decodedBytesVector;
}

std::vector<int8_t> Utilities::boolsToBytes(const bool *const src, const uint32_t &bit_cnt) {
    int byte_cnt = bit_cnt / 8;
    assert(bit_cnt % 8 == 0); // no partial bytes supported

    std::vector<int8_t> result;
    result.resize(byte_cnt);

    bool *cursor = const_cast<bool*>(src);

    for(int i = 0; i < byte_cnt; ++i) {
        result[i] = Utilities::boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}

bool *Utilities::bytesToBool(int8_t *bytes, const int & byte_cnt) {
    bool *ret = new bool[byte_cnt * 8];
    bool *writePos = ret;

    for(int i = 0; i < byte_cnt; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}


signed char Utilities::boolsToByte(const bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}


void Utilities::mkdir(const string &path) {
    Utilities::runCommand("mkdir -p " + path);
}

AggregateId Utilities::getAggregateId(const string &src) {
    if(src ==  "AVG")         return AggregateId::AVG;
    if(src ==  "COUNT")       return AggregateId::COUNT;
    if(src ==  "MIN")         return AggregateId::MIN;
    if(src ==  "MAX")         return AggregateId::MAX;
    if(src ==  "SUM")         return AggregateId::SUM;

    // else
    throw std::invalid_argument("Can't decode aggregate from " + src);

}

uint64_t Utilities::getEpoch() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

string Utilities::printTree(const boost::property_tree::ptree &pt, const std::string &prefix) {
    std::stringstream ss;
    boost::property_tree::ptree::const_iterator end = pt.end();
    for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != end; ++it) {
        ss << prefix <<  it->first << ": " << it->second.get_value<std::string>() << std::endl;
        ss << printTree(it->second, prefix + "   ");
    }

    return ss.str();
}

