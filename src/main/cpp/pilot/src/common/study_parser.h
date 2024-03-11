#ifndef _STUDY_PARSER_
#define _STUDY_PARSER_

#include "catalyst_study.h"

namespace catalyst {
    class StudyParser {
    public:
        StudyParser(const std::string json_config_filename) {
        }

        CatalystStudy parsed_study_;
    };
}
#endif