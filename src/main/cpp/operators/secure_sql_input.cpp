#include "secure_sql_input.h"

std::shared_ptr<SecureTable> SecureSqlInput::runSelf() {
    PsqlDataProvider dataProvider;
    std::shared_ptr<PlainTable> plaintextTable = dataProvider.getQueryTable(dbName, inputQuery, hasDummyTag);

    // secret share it
    output = plaintextTable->secret_share(netio_, srcParty);

    //std::cout << "Secret shared input: " << output->reveal()->toString(true) << std::endl;
    return output;
}

