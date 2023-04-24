#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>
#include <map>
#include "common/defs.h"
#include <iostream>

using namespace std;

// warning: do not include emp-based classes like netio here - conflicts with the static, singleton setup
// creates mysterious compile-time bugs

namespace vaultdb{
    class SystemConfiguration {

    public:

        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        void initialize(const string & db_name, const std::map<ColumnReference, BitPackingDefinition> & bp) {
            unioned_db_name_ = db_name;
            bit_packing_ = bp;
        }

        string getUnionedDbName() const { return unioned_db_name_; }
        BitPackingDefinition getBitPackingSpec(const string & table_name, const string & col_name);
        SystemConfiguration(const SystemConfiguration&) = delete;
        SystemConfiguration& operator=(const SystemConfiguration &) = delete;


    private:
        SystemConfiguration() {}
        string unioned_db_name_;
        std::map<ColumnReference, BitPackingDefinition> bit_packing_;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
