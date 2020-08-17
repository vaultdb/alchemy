//
// Created by Jennie Rogers on 8/15/20.
//

#include <util/emp_manager.h>
#include "secure_sql_input.h"

std::shared_ptr<QueryTable> SecureSqlInput::runSelf() {
    std::shared_ptr<QueryTable> plaintextTable = SqlInput::runSelf();
    EmpManager *empManager = EmpManager::getInstance();

    std::cout << "Secret sharing:\n" << *plaintextTable << std::endl;

    // secret share it
    output = empManager->secretShareTable(plaintextTable.get());
    empManager->flush();



    return output;
}
