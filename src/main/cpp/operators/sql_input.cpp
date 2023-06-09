#include <data/psql_data_provider.h>
#include "sql_input.h"
#include <boost/algorithm/string/replace.hpp>


using namespace vaultdb;

// run this eagerly to get the schema
SqlInput::SqlInput(std::string db, std::string sql, const StorageModel & model, bool dummy_tag)
  :  Operator<bool>(), input_query_(sql), db_name_(db), dummy_tagged_(dummy_tag), storage_model_(model), tuple_limit_(0) {
    runQuery();
    output_schema_ = output_->getSchema();

}

SqlInput::SqlInput(std::string db, std::string sql, bool dummy_tag, const StorageModel & model, const SortDefinition &sort_def, const size_t & tuple_limit) :
        Operator<bool>(sort_def), input_query_(sql), db_name_(db), dummy_tagged_(dummy_tag), storage_model_(model), tuple_limit_(tuple_limit) {

    runQuery();
    output_schema_ = output_->getSchema();
}



// read in the data from supplied SQL query
PlainTable  *SqlInput::runSelf() {


    return output_;


}

void SqlInput::runQuery() {
    PsqlDataProvider dataProvider;

    if(tuple_limit_ > 0) { // truncate inputs
        boost::replace_all(input_query_, ";", "");

        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(tuple_limit_);
    }

    Operator::output_ = dataProvider.getQueryTable(db_name_, input_query_, storage_model_, dummy_tagged_);
    output_->setSortOrder(Operator<bool>::sort_definition_);


}

string SqlInput::getOperatorType() const {
    return "SqlInput";
}

string SqlInput::getParameters() const {
    string str = input_query_;
    std::replace(str.begin(), str.end(), '\n', ' ');
    return "\"" + str + "\", tuple_count=" + std::to_string(output_->getTupleCount());


}

void SqlInput::truncateInput(const size_t &limit) {
    output_->resize(limit);
}


