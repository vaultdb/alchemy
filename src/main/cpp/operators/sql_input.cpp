#include <data/PsqlDataProvider.h>
#include "sql_input.h"

// read in the data from supplied SQL query
std::shared_ptr<QueryTable<BoolField> > SqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable<BoolField> > localOutput = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);
    Operator<BoolField>::output = std::move(localOutput);
    if(!sortedOn.empty()) {  output->setSortOrder(sortedOn); }

    return output;


}
