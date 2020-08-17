//
// Created by Jennie Rogers on 8/15/20.
//

#include <util/emp_manager.h>
#include "encrypted_sql_input.h"

std::shared_ptr<QueryTable> EncryptedSqlInput::runSelf() {
    std::shared_ptr<QueryTable> plaintextTable = SqlInput::runSelf();
    EmpManager *empManager = EmpManager::getInstance();


    // secret share it
    std::unique_ptr<QueryTable> encryptedTable = empManager->secretShareTable(plaintextTable.get());
    empManager->flush();



}
