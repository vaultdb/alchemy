#include "zk_sql_input.h"

ZkSqlInput::ZkSqlInput(string db, string sql, bool dummy_tag,  const size_t &input_tuple_limit) :
                                                                                     input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());
}

ZkSqlInput::ZkSqlInput(const string &db, const string &sql, const bool &dummy_tag,
                               const SortDefinition &sort_def,  const size_t & input_tuple_limit) :
        Operator(sort_def),input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());

}


SecureTable *ZkSqlInput::runSelf() {
    // secret share it
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = emp::CircuitExecution::circ_exec->num_and();

    output_ = plain_input_->secretShare();
    return output_;
}



void ZkSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_);
    plain_input_->setSortOrder(getSortOrder());
}

string ZkSqlInput::getOperatorType() const {
    return "ZkSqlInput";
}

string ZkSqlInput::getParameters() const {
    return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
}


