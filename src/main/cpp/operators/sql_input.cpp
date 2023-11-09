#include <data/psql_data_provider.h>
#include "sql_input.h"
#include <boost/algorithm/string/replace.hpp>


using namespace vaultdb;

// run this eagerly to get the schema
SqlInput::SqlInput(std::string db, std::string sql, bool dummy_tag)
  :  Operator<bool>(), input_query_(sql), original_input_query_(sql), db_name_(db),   dummy_tagged_(dummy_tag), tuple_limit_(0) {
    runQuery();
    output_schema_ = output_->getSchema();
    this->output_cardinality_ = this->output_->tuple_cnt_;

}

SqlInput::SqlInput(std::string db, std::string sql, bool dummy_tag, const SortDefinition &sort_def,
                   const size_t &tuple_limit) :
        Operator<bool>(sort_def), input_query_(sql), original_input_query_(sql),  db_name_(db), original_collation_(sort_def), dummy_tagged_(dummy_tag),  tuple_limit_(tuple_limit){

    runQuery();
    output_schema_ = output_->getSchema();
    this->output_cardinality_ = this->output_->tuple_cnt_;
}


// read in the data from supplied SQL query
PlainTable  *SqlInput::runSelf() {

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    if(this->output_ == nullptr || this->output_->order_by_ != this->sort_definition_) {
        runQuery();
    }

    return output_;


}

void SqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    string sql = input_query_;

    if(tuple_limit_ > 0) { // truncate inputs
        boost::replace_all(input_query_, ";", "");

        sql = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(tuple_limit_);
    }

    output_ = dataProvider.getQueryTable(db_name_, sql, dummy_tagged_);
    output_->order_by_ = this->sort_definition_;


}



void SqlInput::truncateInput(const size_t &limit) {
    output_->resize(limit);
}


