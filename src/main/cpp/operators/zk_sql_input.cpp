#include "operators/zk_sql_input.h"

ZkSqlInput::ZkSqlInput(string db, string sql, bool dummy_tag,  const size_t &input_tuple_limit) :
                                                                                     input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());

    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->tuple_cnt_);

}

ZkSqlInput::ZkSqlInput(const string &db, const string &sql, const bool &dummy_tag,
                               const SortDefinition &sort_def,  const size_t & input_tuple_limit) :
        Operator(sort_def),input_query_(sql), db_name_(db), has_dummy_tag_(dummy_tag), input_tuple_limit_(input_tuple_limit) {

    runQuery();
    output_schema_ = QuerySchema::toSecure(plain_input_->getSchema());

    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    this->output_cardinality_ = manager->getTableCardinality(plain_input_->tuple_cnt_);
}


SecureTable *ZkSqlInput::runSelf() {
    // commit it
    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    output_ = plain_input_->secretShare();
    return output_;
}



void ZkSqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    if(input_tuple_limit_ > 0) { // truncate inputs
        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(input_tuple_limit_);
    }

    plain_input_ = dataProvider.getQueryTable(db_name_, input_query_);
    plain_input_->order_by_ = this->sort_definition_;
}




