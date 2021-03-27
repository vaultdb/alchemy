#include "secure_sql_input.h"

std::shared_ptr<SecureTable> SecureSqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::unique_ptr<PlainTable> plaintextTable = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);

    // secret share it
    output = plaintextTable->secretShare(netio_, srcParty);
    if(!sortedOn.empty()) {  output->setSortOrder(sortedOn); }


    return output;
}

