#include <data/csv_reader.h>
#include "csv_input.h"

using namespace  vaultdb;


PlainTable *CsvInput::runSelf() {
    Operator::output_ = CsvReader::readCsv(input_file_, output_schema_);
    return Operator::output_;
}

string CsvInput::getOperatorType() const {
    return "CsvInput";
}

string CsvInput::getParameters() const {
    return "filename=" + input_file_;
}

