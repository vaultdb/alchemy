#include <util/emp_manager.h>
#include "secure_sql_input.h"

std::shared_ptr<QueryTable> SecureSqlInput::runSelf() {
    std::shared_ptr<QueryTable> plaintextTable = SqlInput::runSelf();


    // secret share it
    output = plaintextTable->secretShare(netio_, srcParty);




    return output;
}
