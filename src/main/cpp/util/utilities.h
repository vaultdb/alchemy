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
#include <operators/support/aggregate_id.h>
#include <common/defs.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <gflags/gflags.h>
#include "system_configuration.h"


namespace vaultdb {


    class Utilities {
    public:

        static std::string getCurrentWorkingDirectory();
        static void checkMemoryUtilization(const std::string & msg);

        static size_t checkMemoryUtilization(bool print=false);

        static size_t residentMemoryUtilization(bool print=false);

        static void checkMemoryUtilization();

        static size_t checkSwapUtilization(bool print=false);

        static size_t checkMemoryAndSwapUtilization();

        static void checkDiskIOUtilization();

        static std::string getStackTrace();

        static bool *bytesToBool(int8_t *bytes, const int &  byte_cnt);

        static std::vector<int8_t> boolsToBytes(const bool *const src, const uint32_t &bit_cnt);

        static std::vector<int8_t> boolsToBytes( std::string & src); // for the output of Integer::reveal<string>()

        // convert 8 bits to a byte
        static int8_t boolsToByte(const bool *src);

        static std::string revealAndPrintBytes(emp::Bit *bits, const int &byte_cnt);

        static void mkdir(const std::string & path);

        static inline bool If(const bool & cond, const bool & lhs, const bool & rhs) { return (cond) ? lhs : rhs; }
        static inline emp::Bit If(const emp::Bit & cond, const emp::Bit & lhs, const emp::Bit & rhs) { return emp::If(cond, lhs, rhs); }

        // for use in plan reader
        static AggregateId getAggregateId(const std::string & src);
        static uint64_t getEpoch();



        // for parser debugging
        static string printTree(const boost::property_tree::ptree &pt, const std::string &prefix = "");

        // for simulating types in JSON deparser

        static string eraseValueQuotes(const string & json_plan, const vector<string> & search_strs)  {
            // iterate over JSON plan and delete extra quotes around some fields
            vector<string> lines;
            boost::split(lines, json_plan, boost::is_any_of("\n"));


            for(auto search_str : search_strs) {
                for(auto & line : lines) {
                if(line.find(search_str) != std::string::npos) {
                    // delete last two instances of quote
                    line.erase(line.find_last_of("\""), 1);
                    line.erase(line.find_last_of("\""), 1);
                }
            }
        }

            // re-assemble lines
            string output;
            for(auto line : lines) {
                output += line + "\n";
            }
            return output;

        }

        template<typename T>
        static bool vectorEquality(const vector<T> & lhs, const vector<T> & rhs)  {
            if(lhs.size() != rhs.size()) return false;
            for(size_t i = 0; i < lhs.size(); ++i) {
                if(lhs[i] != rhs[i]) return false;
            }
            return true;
        }

        static void reverseString(char *s, const int & len) {
           char *l = s;
           char *r = s + len - 1;

            // swap in place
            while(l < r) {
                *l = *l ^ *r;
                *r = *r ^ *l;
                *l = *l ^ *r;

                ++l;
                --r;
            }
        }

        // https://stackoverflow.com/questions/52164723/how-to-execute-a-command-and-get-return-code-stdout-and-stderr-of-command-in-c
        static string runCommand(const string & cmd, const string & cwd = "") {
            string dir = (cwd == "") ?  Utilities::getCurrentWorkingDirectory() : cwd;

            string to_run = "cd " + dir + " && " + cmd + " 2>&1 ";

            std::array<char, 128> buffer;
            std::string result;
            auto pipe = popen(to_run.c_str(), "r");
            if (!pipe) throw std::runtime_error("popen() failed!");

            while (!feof(pipe)) {
                if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
                    result += buffer.data();
            }

            auto rc = pclose(pipe);

            if (rc == EXIT_SUCCESS) { // == 0
                return result;
            }

            return "";



        }

        // from https://stackoverflow.com/questions/13172158/c-split-string-by-line
        static vector<string> splitStringByNewline(const std::string& str)
        {
            auto result = std::vector<std::string>{};
            auto ss = std::stringstream{str};

            for (std::string line; std::getline(ss, line, '\n');)
                result.push_back(line);

            return result;
        }

        // erases first instance of to_search
        static string eraseSubstring(string src, string to_search) {
            auto pos = src.find(to_search);
            if(pos != std::string::npos) {
                src.erase(pos, to_search.size());
            }
            return src;
        }

        static string getGFlags() {
            string settings = gflags::CommandlineFlagsIntoString();
            std::replace(settings.begin(), settings.end(), '\n', '\t'); // replace all endline with tabs
            // delete unused flags (defaults from GFlags)
            //  --flagfile=flagfiles/storage.flags
            //  --alice_db=tpch_alice_150       --alice_host=127.0.0.1  --bob_db=tpch_bob_150   --ctrl_port=65453 --cutoff=5       --filter=*      --party=1       --port=43446    --storage=compressed    --unioned_db=tpch_unioned_150   --validation=true       --log_level=2   --logfile=
            settings = eraseSubstring(settings, "--fromenv=\t");
            settings = eraseSubstring(settings, "--tryfromenv=\t");
            settings = eraseSubstring(settings, "--undefok=\t");
            settings = eraseSubstring(settings, "--tab_completion_columns=80\t");
            settings = eraseSubstring(settings, "--tab_completion_word=\t");
            settings = eraseSubstring(settings, "--help=false\t");
            settings = eraseSubstring(settings, "--helpfull=false\t");
            settings = eraseSubstring(settings, "--helpmatch=\t");
            settings = eraseSubstring(settings, "--helpon=\t");
            settings = eraseSubstring(settings, "--helppackage=false\t");
            settings = eraseSubstring(settings, "--helpshort=false\t");
            settings = eraseSubstring(settings, "--helpxml=false\t");
            settings = eraseSubstring(settings, "--version=false\t");

            return settings;
        }

        static string getStorageModeStr(const StorageModel & storage_model) {
            switch (storage_model) {
                case StorageModel::COLUMN_STORE:
                    return "column store";
                case StorageModel::PACKED_COLUMN_STORE:
                    return "packed wires";
                case StorageModel::COMPRESSED_STORE:
                    return "compressed";
                case StorageModel::ROW_STORE:
                    return "row store";
                default:
                    return "unknown";
            }
        }

        static string getTestParameters() {
            std::stringstream  output;
            string code_version = Utilities::runCommand("git rev-parse HEAD");
            if(code_version[code_version.size()-1] != '\n') code_version += "\n";

            SystemConfiguration & s = SystemConfiguration::getInstance();
            output << "EMP mode: " << EmpManager::empModeString(s.emp_mode_) << ", storage mode: " << getStorageModeStr(s.storageModel()) << endl;

            output << "Code version: " << code_version;
            output << "GFlags: " <<  Utilities::getGFlags() << endl;
            return output.str();
        }

        static string getFullyQualifiedPath(const string & relative_path) {
            string cwd = getCurrentWorkingDirectory();
            if(relative_path[0] == '/') return relative_path;
            return cwd + "/" + relative_path;
        }

        static inline string getFilenameForTable(const string & table_name) {
            return SystemConfiguration::getInstance().stored_db_path_ + "/" + table_name + "." + std::to_string(SystemConfiguration::getInstance().party_);
        }

    };


}

#endif //_UTILITIES_H
