#include <data/CsvReader.h>
#include "csv_input.h"

using namespace  vaultdb;


std::shared_ptr<PlainTable > CsvInput::runSelf() {
    Operator::output_ = CsvReader::readCsv(inputFile, inputSchema);
    return Operator::output_;
}

