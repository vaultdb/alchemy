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
        ~Catalyst() {
            if(parser_ != nullptr) {
                delete parser_;
            }
        } // emp_manager is deleted by SystemConfiguration

        void loadStudyData();
        void runQueries();
        CatalystStudy<Bit> getStudy() const { return parser_->study_; }

    private:
        bool data_loaded_ = false;
        StudyParser *parser_ = nullptr;
        TableManager & table_manager_ = TableManager::getInstance();

        void importSecretShares(const string & table_name, const int & src_party);

    };
}
#endif
