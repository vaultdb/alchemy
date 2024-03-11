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

        CatalystStudy<Bit> study_;
    };
}
#endif