//
// Created by Jennie Rogers on 8/15/20.
//

#include <data/PsqlDataProvider.h>
#include <util/emp_manager.h>
#include "sql_input.h"

// read in the data from supplied SQL query
std::shared_ptr<QueryTable> SqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> localOutput = dataProvider.GetQueryTable(dbName, inputQuery, hasDummyTag);

    output = std::move(localOutput);
    return output;
}
