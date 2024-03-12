#ifndef _CATALYST_
#define _CATALYST_

#include <string>
#include "common/study_parser.h"
#include "util/table_manager.h"

using namespace std;
using namespace vaultdb;


namespace catalyst {
    class Catalyst {
    public:
        Catalyst(int party, const std::string json_config_filename);
        ~Catalyst() { } // emp_manager is deleted by SystemConfiguration

    private:
        CatalystStudy<Bit> study_;
        TableManager & table_manager_ = TableManager::getInstance();

        void importSecretShares(const string & table_name, const int & src_party);

    };
}
#endif
