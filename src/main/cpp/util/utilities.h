#ifndef VAULTDB_UTILITIES_H
#define VAULTDB_UTILITIES_H

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
#include <operators/support/aggregate_id.h>
#include <common/defs.h>
#include <util/logger.h>
#include <boost/property_tree/ptree.hpp>

namespace vaultdb {


    class Utilities {
    public:

        static std::string getCurrentWorkingDirectory();
        static void checkMemoryUtilization(const std::string & msg);

        static void checkMemoryUtilization();

        static std::string getStackTrace();

        static bool *bytesToBool(int8_t *bytes, const int &  byteCount);

        static std::vector<int8_t> boolsToBytes(const bool *const src, const uint32_t &bitCount);

        static std::vector<int8_t> boolsToBytes( std::string & src); // for the output of Integer::reveal<string>()

        // convert 8 bits to a byte
        static int8_t boolsToByte(const bool *src);

        static std::string revealAndPrintBytes(emp::Bit *bits, const int &byteCount);

        static void mkdir(const std::string & path);

        static inline bool If(const bool & cond, const bool & lhs, const bool & rhs) { return (cond) ? lhs : rhs; }
        static inline emp::Bit If(const emp::Bit & cond, const emp::Bit & lhs, const emp::Bit & rhs) { return emp::If(cond, lhs, rhs); }

        // for use in plan reader
        static AggregateId getAggregateId(const std::string & src);
        static uint64_t getEpoch();



        // for parser debugging
        static void printTree(const boost::property_tree::ptree &pt, const std::string &prefix = "");

    };

}

#endif //_UTILITIES_H
