#include "plain_base_test.h"
#include <emp-tool/emp-tool.h>
#include <util/system_configuration.h>
#include "util/field_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include "util/emp_manager/sh2pc_manager.h"

#if __has_include("emp-rescu/emp-rescu.h")
static EmpMode _emp_mode_ = EmpMode::OUTSOURCED;
#elif  __has_include("emp-sh2pc/emp-sh2pc.h")
static EmpMode _emp_mode_ = EmpMode::SH2PC;
#else
    static EmpMode _emp_mode_ = EmpMode::PLAIN;
#endif


void PlainBaseTest::SetUp()  {
    assert(FLAGS_storage == "row" || FLAGS_storage == "column");
    storage_model_ = (FLAGS_storage == "row") ? StorageModel::ROW_STORE : StorageModel::COLUMN_STORE;

    std::cout << "Received storage flag of " << FLAGS_storage << '\n';

    if(_emp_mode_ == EmpMode::OUTSOURCED) {
        manager_ = new OutsourcedMpcManager();
    } else if(_emp_mode_ == EmpMode::SH2PC) {
        manager_ = new SH2PCManager();
    } else {
        throw std::runtime_error("Unsupported EMP mode");
    }

    current_working_directory_ = Utilities::getCurrentWorkingDirectory();
    SystemConfiguration &s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(db_name_);
    s.initialize(db_name_, md, storage_model_);
    s.emp_manager_ = manager_;
};

void PlainBaseTest::TearDown() {
    // teardown covered in SystemConfiguration destructor

}
