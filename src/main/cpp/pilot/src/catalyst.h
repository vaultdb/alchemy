#ifndef _CATALYST_
#define _CATALYST_

#include <string>
#include "common/study_parser.h"

using namespace std;
using namespace vaultdb;


namespace catalyst {
    class Catalyst {
    public:
        Catalyst(const std::string json_config_filename)  {

            StudyParser study_parser(json_config_filename);
            study_ = study_parser.study_;
        }

    private:
        CatalystStudy<Bit> study_;
    };
}
#endif
