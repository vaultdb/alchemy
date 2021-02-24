#ifndef _UTILITIES_H
#define _UTILITIES_H

// designed to be minimalist, standalone procedures
// circumvents a dependency loop

// For EMP memory instrumentation
#if defined(__linux__)
#include <sys/time.h>
#include <sys/resource.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#endif

#include <cstdint>
#include <vector>
#include <string>

using namespace std;

namespace vaultdb {
    class Utilities {
    public:

        static std::string getCurrentWorkingDirectory();

        static void checkMemoryUtilization(string msg);

        static void checkMemoryUtilization();

        static string getStackTrace();

        static bool *bytesToBool(int8_t *bytes, int byteCount);

        static vector<int8_t> boolsToBytes(const bool *const src, const uint32_t &bitCount);

        static vector<int8_t> boolsToBytes( string & src); // for the output of Integer::reveal<string>()

        // convert 8 bits to a byte
        static int8_t boolsToByte(const bool *src);


    };

}

#endif //_UTILITIES_H
