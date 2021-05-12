#include <data/PsqlDataProvider.h>
#include "sql_input.h"
#include <boost/algorithm/string/replace.hpp>


using namespace vaultdb;

// run this eagerly to get the schema
SqlInput::SqlInput(std::string db, std::string sql, bool dummyTag) :   input_query_(sql), db_name_(db), dummy_tagged_(dummyTag), tuple_limit_(0) {
    runQuery();
    output_schema_ = *output_->getSchema();

}

SqlInput::SqlInput(std::string db, std::string sql, bool dummyTag, const SortDefinition &sortDefinition, const size_t & tuple_limit) :
        Operator<bool>(sortDefinition), input_query_(sql), db_name_(db), dummy_tagged_(dummyTag), tuple_limit_(tuple_limit) {

    runQuery();
    output_schema_ = *output_->getSchema();
}



// read in the data from supplied SQL query
std::shared_ptr<PlainTable > SqlInput::runSelf() {


    return output_;


}

void SqlInput::runQuery() {
    PsqlDataProvider dataProvider;

    if(tuple_limit_ > 0) { // truncate inputs
        boost::replace_all(input_query_, ";", "");

        input_query_ = "SELECT * FROM (" + input_query_ + ") input LIMIT " + std::to_string(tuple_limit_);
    }

    std::shared_ptr<PlainTable> localOutput = dataProvider.getQueryTable(db_name_, input_query_, dummy_tagged_);
    Operator::output_ = std::move(localOutput);
    output_->setSortOrder(Operator<bool>::sort_definition_);

    if(output_->getTupleCount() <= 0) {
        throw std::invalid_argument("read empty input from \"" + input_query_ + "\"");
    }


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


