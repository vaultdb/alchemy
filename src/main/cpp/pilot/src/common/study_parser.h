#ifndef _STUDY_PARSER_
#define _STUDY_PARSER_

#include "catalyst_study.h"
#include <string>
#include <tuple>
#include <map>
#include <boost/property_tree/ptree.hpp>


namespace catalyst {
    class StudyParser {
    public:
        // takes as input a filename that contains the study parameters
        // path is relative to $VAULTDB_ROOT/src/main/cpp
        StudyParser(const std::string plan_filename);


        // main study - includes input tables, queries, and secret shares root
        CatalystStudy<Bit> study_;

        int port_;
        string alice_host_;
        string protocol_ = "sh2pc";
        // temp cache:
        // these are params that we need until we set up MPC
        // then we read secret shares
        // then we can read the per-query JSON files and parse them
         std::map<std::string, std::string> query_files_;
    };
}
#endif