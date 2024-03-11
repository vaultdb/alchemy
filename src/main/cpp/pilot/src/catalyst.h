#ifndef _CATALYST_
#define _CATALYST_

#include <string>
#include "common/study_parser.h"

using namespace std;
using namespace vaultdb;


namespace catalyst {
    class Catalyst {
    public:
        Catalyst(const std::string json_config_filename) : json_config_filename_(json_config_filename) {

        }

    private:
        string json_config_filename_;
    };
}
#endif
