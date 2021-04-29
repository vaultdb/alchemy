#include <data/PsqlDataProvider.h>
#include "sql_input.h"

// read in the data from supplied SQL query
std::shared_ptr<PlainTable > SqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> localOutput = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    Operator::output_ = std::move(localOutput);
    if(!sortedOn.empty()) {  output_->setSortOrder(sortedOn); }

    return output_;


}
