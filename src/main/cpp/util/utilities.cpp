#include "utilities.h"

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>
#include <sstream>
#include <iostream>

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

using namespace vaultdb;

std::string Utilities::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    char * ret = getcwd(cwd, sizeof(cwd));
    ++ret; // for compile-time errors

    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
}






// From Chenkai Li's EMP memory instrumentation

void Utilities::checkMemoryUtilization(const string & msg) {
    cout << "Checking memory utilization after " << msg << endl;
    Utilities::checkMemoryUtilization();
}

void Utilities::checkMemoryUtilization() {
#if defined(__linux__)
    struct rusage rusage;
	if (!getrusage(RUSAGE_SELF, &rusage))
		std::cout << "[Linux]Peak resident set size: " << (size_t)rusage.ru_maxrss <<  " bytes." << std::endl;
	else std::cout << "[Linux]Query RSS failed" << std::endl;
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
        std::cout << "[Mac]Peak resident set size: " << (size_t)info.resident_size_max << " bytes, current memory size: " << (size_t)info.resident_size  <<  std::endl;
    else std::cout << "[Mac]Query RSS failed" << std::endl;
#endif

}


string Utilities::getStackTrace() {
    std::ostringstream  os;
    os <<  boost::stacktrace::stacktrace();
    return os.str();
}

vector<int8_t> Utilities::boolsToBytes( string &bitString) {
    int srcBits = bitString.length();
    string::iterator strPos = bitString.begin();
    bool *bools = new bool[srcBits];

    for(int i =  0; i < srcBits; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, srcBits);
    delete[] bools;
    return decodedBytesVector;
}

vector<int8_t> Utilities::boolsToBytes(const bool *const src, const uint32_t &bitCount) {
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
