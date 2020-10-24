//
// Created by Jennie Rogers on 8/15/20.
//

#include <util/emp_manager.h>
#include "secure_sql_input.h"

std::shared_ptr<QueryTable> SecureSqlInput::runSelf() {
    std::shared_ptr<QueryTable> plaintextTable = SqlInput::runSelf();


    // secret share it
    output = EmpManager::secretShareTable(plaintextTable.get(), netio_, dstParty);




    return output;
}
