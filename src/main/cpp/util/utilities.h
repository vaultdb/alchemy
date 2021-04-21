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
#include <emp-tool/circuits/bit.h>
#include <support/aggregate_id.h>


namespace vaultdb {
    class Utilities {
    public:

        static std::string getCurrentWorkingDirectory();

        static void checkMemoryUtilization(const std::string & msg);

        static void checkMemoryUtilization();

        static std::string getStackTrace();

        static bool *bytesToBool(int8_t *bytes, int byteCount);

        static std::vector<int8_t> boolsToBytes(const bool *const src, const uint32_t &bitCount);

        static std::vector<int8_t> boolsToBytes( std::string & src); // for the output of Integer::reveal<string>()

        // convert 8 bits to a byte
        static int8_t boolsToByte(const bool *src);

        static std::string revealAndPrintBytes(emp::Bit *bits, const int &byteCount);

        static void mkdir(const std::string & path);

        // for use in plan reader
        static AggregateId getAggregateId(const std::string & src);



    };

}

#endif //_UTILITIES_H
