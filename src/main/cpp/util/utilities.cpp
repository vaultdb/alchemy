#include "utilities.h"

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <chrono>
#include <boost/stacktrace.hpp>
#include <iostream>
#include <filesystem>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif



using namespace std::chrono;
using namespace vaultdb;

std::string Utilities::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    char * ret = getcwd(cwd, sizeof(cwd));
    ++ret; // for compile-time warnings

    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
}






// From Chenkai Li's EMP memory instrumentation

void Utilities::checkMemoryUtilization(const std::string & msg, const logging::trivial::severity_level & severity) {
    Logger::write("Checking memory utilization after " + msg, severity);
    Utilities::checkMemoryUtilization();
}

void Utilities::checkMemoryUtilization(const logging::trivial::severity_level & severity) {
    auto logger = vaultdb_logger::get();

  // get current time in epoch
  uint64_t epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    

#if defined(__linux__)
    struct rusage rusage;
	if (!getrusage(RUSAGE_SELF, &rusage))
	  BOOST_LOG_SEV(logger, severity) << "[Linux]Peak resident set size: " << (size_t)rusage.ru_maxrss <<  " bytes, at epoch " <<  epoch << " ms" <<  std::endl;
	else
        Logger::write("[Linux]Query RSS failed", severity);
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
        BOOST_LOG_SEV(logger, severity) << "[Mac]Peak resident set size: " << (size_t)info.resident_size_max << " bytes, current memory size: " << (size_t)info.resident_size  <<  " at epoch " << epoch << " ms" << std::endl;
    else Logger::write("[Mac]Query RSS failed", severity);
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

std::vector<int8_t> Utilities::boolsToBytes(const bool *const src, const uint32_t &bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    std::vector<int8_t> result;
    result.resize(byteCount);

    bool *cursor = const_cast<bool*>(src);

    for(int i = 0; i < byteCount; ++i) {
        result[i] = Utilities::boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}

bool *Utilities::bytesToBool(int8_t *bytes, int byteCount) {
    bool *ret = new bool[byteCount * 8];


    bool *writePos = ret;

    for(int i = 0; i < byteCount; ++i) {
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


string Utilities::revealAndPrintBytes(emp::Bit *bits, const int &byteCount) {
    std::stringstream ss;

    for(int i = 0; i < byteCount * 8; ++i) {
        ss << bits[i].reveal();
        if((i+1) % 8 == 0) std::cout << " ";
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


