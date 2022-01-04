#include "plain_base_test.h"
#include <emp-tool/emp-tool.h>

void PlainBaseTest::SetUp()  {
  setup_plain_prot(false, "");
    Logger::setup();
    currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();

};

void PlainBaseTest::TearDown() {

    finalize_plain_prot();
};
