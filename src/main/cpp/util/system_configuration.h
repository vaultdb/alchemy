#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>

using namespace std;
namespace vaultdb{
    class SystemConfiguration {

    public:

        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        void setUnionedDbName(string db_name) {unioned_db_name_ = db_name; }
        string getUnionedDbName() const { return unioned_db_name_; }

    private:
        SystemConfiguration() {}
        string unioned_db_name_;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
