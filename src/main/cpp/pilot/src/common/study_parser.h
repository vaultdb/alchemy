#ifndef _STUDY_PARSER_
#define _STUDY_PARSER_

#include "catalyst_study.h"
#include <string>
#include <tuple>
#include <map>
#include "parser/plan_parser.h"
#include <boost/property_tree/ptree.hpp>


namespace catalyst {
    class StudyParser {
    public:
        // takes as input a filename that contains the study parameters
        // path is relative to $VAULTDB_ROOT/src/main/cpp
        StudyParser(const std::string plan_filename);

        void parseQueries() {
            for(auto q : query_files_) {
                study_.queries_[q.first] = PlanParser<Bit>::parse("", q.second);
            }
        }

        // main study - includes input tables, queries, and secret shares root
        CatalystStudy<Bit> study_;

        int port_;
        string alice_host_;
        string protocol_ = "sh2pc";
        std::map<std::string, std::string> query_files_;
    };
}
#endif