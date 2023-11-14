#include "plain_base_test.h"
#include <emp-tool/emp-tool.h>
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/emp_manager/sh2pc_manager.h"

#if  __has_include("emp-sh2pc/emp-sh2pc.h")
static EmpMode _emp_mode_ = EmpMode::SH2PC;
#elif __has_include("emp-zk/emp-zk.h")
static EmpMode _emp_mode_ = EmpMode::ZK;
#elif __has_include("emp-rescu/emp-rescu.h")
    static EmpMode _emp_mode_ = EmpMode::OUTSOURCED;
#else
    static EmpMode _emp_mode_ = EmpMode::PLAIN;
#endif


void PlainBaseTest::SetUp()  {

    std::cout << "Received emp_mode: " << EmpManager::empModeString(_emp_mode_) << std::endl;

    if(_emp_mode_ == EmpMode::OUTSOURCED) {
        manager_ = new OutsourcedMpcManager();
    } else if(_emp_mode_ == EmpMode::SH2PC) {
        manager_ = new SH2PCManager();
    } else {
        throw std::runtime_error("Unsupported EMP mode");
    }

    current_working_directory_ = Utilities::getCurrentWorkingDirectory();
    SystemConfiguration &s = SystemConfiguration::getInstance();
    s.emp_mode_ = vaultdb::EmpMode::PLAIN; // only need _emp_mode_ to initialize EmpManager
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(db_name_);
    s.initialize(db_name_, md, storage_model_);
    s.emp_manager_ = manager_;
};

void PlainBaseTest::TearDown() {
    // teardown covered in SystemConfiguration destructor
    delete manager_;
    SystemConfiguration::getInstance().emp_manager_ = nullptr;
}
