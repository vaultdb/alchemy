//
// Created by Jennie Rogers on 8/15/20.
//

#include <data/PsqlDataProvider.h>
#include "sql_input.h"

// read in the data from supplied SQL query
std::shared_ptr<QueryTable> SqlInput::runSelf() {
    // initialized in constructor
    return output;
}
