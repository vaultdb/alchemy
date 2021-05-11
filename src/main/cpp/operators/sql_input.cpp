#include <data/PsqlDataProvider.h>
#include "sql_input.h"

using namespace vaultdb;

// run this eagerly to get the schema
SqlInput::SqlInput(std::string db, std::string sql, bool dummyTag) :   inputQuery(sql), dbName(db), hasDummyTag(dummyTag) {
    runQuery();
    output_schema_ = *output_->getSchema();

}

SqlInput::SqlInput(std::string db, std::string sql, bool dummyTag, const SortDefinition &sortDefinition) :
        Operator<bool>(sortDefinition), inputQuery(sql), dbName(db), hasDummyTag(dummyTag) {

    runQuery();
    output_schema_ = *output_->getSchema();
}



// read in the data from supplied SQL query
std::shared_ptr<PlainTable > SqlInput::runSelf() {


    return output_;


}

void SqlInput::runQuery() {
    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> localOutput = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    Operator::output_ = std::move(localOutput);
    output_->setSortOrder(Operator<bool>::sort_definition_);

}

string SqlInput::getOperatorType() const {
    return "SqlInput";
}

string SqlInput::getParameters() const {
    string str = inputQuery;
    std::replace(str.begin(), str.end(), '\n', ' ');
    return "\"" + str + "\", tuple_count=" + std::to_string(output_->getTupleCount());
    assert(output_->getTupleCount() > 0); // can't run a query without data!
}

void SqlInput::truncateInput(const size_t &limit) {
    output_->resize(limit);
}


