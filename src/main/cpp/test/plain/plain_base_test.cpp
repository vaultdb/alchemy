#include "plain_base_test.h"
#include <emp-tool/emp-tool.h>
#include <util/system_configuration.h>
#include "util/field_utilities.h"

void PlainBaseTest::SetUp()  {
    assert(FLAGS_storage == "row" || FLAGS_storage == "column");
    storage_model_ = (FLAGS_storage == "row") ? StorageModel::ROW_STORE : StorageModel::COLUMN_STORE;


    setup_plain_prot(false, "");
    //Logger::setup(); // write to console
    //Logger::setup("vaultdb");
    current_working_directory_ = Utilities::getCurrentWorkingDirectory();
    SystemConfiguration &s = SystemConfiguration::getInstance();
    BitPackingMetadata md = FieldUtilities::getBitPackingMetadata(db_name_);
    s.initialize(db_name_, md);
};

void PlainBaseTest::TearDown() {

    finalize_plain_prot();
};
