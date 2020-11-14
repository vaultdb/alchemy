#include <data/PsqlDataProvider.h>
#include "sql_input.h"

// read in the data from supplied SQL query
std::shared_ptr<QueryTable> SqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> localOutput = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    output = std::move(localOutput);
    if(!sortedOn.empty()) {  output->setSortOrder(sortedOn); }

    return output;


}
