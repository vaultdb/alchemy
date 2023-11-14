#include "utilities.h"

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <chrono>
#include <boost/stacktrace.hpp>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <common/defs.h>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif



using namespace std::chrono;
using namespace vaultdb;

std::string Utilities::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
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


string Utilities::revealAndPrintBytes(emp::Bit *bits, const int &byte_cnt) {
    std::stringstream ss;

    for(int i = 0; i < byte_cnt * 8; ++i) {
        ss << bits[i].reveal();
        if((i+1) % 8 == 0) ss << " ";
    }

    return ss.str();
}

void Utilities::mkdir(const string &path) {
    std::filesystem::create_directory(path);
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

