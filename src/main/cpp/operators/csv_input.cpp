#include <data/csv_reader.h>
#include "csv_input.h"

using namespace  vaultdb;


PlainTable *CsvInput::runSelf() {
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    this->output_ = CsvReader::readCsv(input_file_, output_schema_);
    this->output_cardinality_ = this->output_->getTupleCount();
    return this->output_;
}


