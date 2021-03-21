#include <data/CsvReader.h>
#include "csv_input.h"

using namespace  vaultdb;


std::shared_ptr<QueryTable<BoolField> > CsvInput::runSelf() {
    Operator<BoolField>::output = CsvReader::readCsv(inputFile, inputSchema);
    return Operator<BoolField>::output;
}

