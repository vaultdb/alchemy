#include <data/CsvReader.h>
#include "csv_input.h"

using namespace  vaultdb;

std::shared_ptr<QueryTable> CsvInput::runSelf() {
    output = CsvReader::readCsv(inputFile, inputSchema);
    return output;
}
